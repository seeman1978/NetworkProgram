//
// Created by qiangwang on 2021/9/15.
//

#include <unistd.h>
#include "../unp.h"

void child_main(int i, int listenfd, int addrlen){
    int connfd;
    void web_child(int);
    socklen_t clilen;
    struct sockaddr *cliaddr;
    extern long		*cptr;
    fd_set	rset;
    cliaddr = static_cast<sockaddr *>(Malloc(addrlen));
    printf("child %ld starting\n", (long)getpid());
    void my_lock_wait();
    void my_lock_release();
    for (;;){
        clilen = addrlen;
        my_lock_wait();
        connfd = Accept(listenfd, cliaddr, &clilen);
        my_lock_release();
        cptr[i]++;
        web_child(connfd);
        Close(connfd);
    }
}
pid_t child_make(int i, int listenfd, int addrlen){
    pid_t pid;
    if ((pid = Fork()) > 0){
        return pid; /// parent
    }
    child_main(i, listenfd, addrlen);    ///invoked by child process and never returns
}