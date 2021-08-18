//
// Created by qiangwang on 2021/8/18.
//

#include <csignal>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "../unp.h"
void str_cli(FILE* fp, int sockfd){
    pid_t pid;
    char sendline[MAXLINE], recvline[MAXLINE];
    if ((pid = Fork()) == 0){/// child: server -> stdout
        while(Readline(sockfd, recvline, MAXLINE) > 0){
            Fputs(recvline, stdout);
        }
        kill(getppid(), SIGTERM); /// in case parent still running
        exit(0);
    }
    /// parent: stdin -> server
    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        Writen(sockfd, sendline, strlen(sendline));
    }
    Shutdown(sockfd, SHUT_WR); /// EOF on stdin, send FIN
    pause();
    return;
}
