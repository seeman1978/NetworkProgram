//
// Created by qiangwang on 2021/9/17.
//

#include <unistd.h>
#include "../unp.h"
#include "child.h"

void child_main(int i, int listenfd, int addrlen){
    char c;
    int connfd;
    ssize_t n;
    void web_child(int);

    printf("child %ld starting\n", (long)getpid());

    for (;;){
        if ((n = Read_fd(STDERR_FILENO, &c, 1, &connfd)) == 0){
            err_quit("read fd returned 0");
        }
        if (connfd < 0){
            err_quit("no descriptor form read_fd");
        }

        web_child(connfd);
        Close(connfd);
        Write(STDERR_FILENO, (void *) "", 1);
    }
}

pid_t child_make(int i, int listenfd, int addrlen){
    int sockfd[2];
    pid_t pid;

    Socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

    if ((pid = Fork()) > 0){
        Close(sockfd[1]);
        cptr[i].child_pid = pid;
        cptr[i].child_pipefd = sockfd[0];
        cptr[i].child_status = 0;
        return pid; /// parent
    }
    Dup2(sockfd[1], STDERR_FILENO);     ///child's stream pipe to parent
    Close(sockfd[0]);
    Close(sockfd[0]);
    Close(listenfd);                    /// child does not need this open
    child_main(i, listenfd, addrlen);   ///invoked by child process and never returns
}