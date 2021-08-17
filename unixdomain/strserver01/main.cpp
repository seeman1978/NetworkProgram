#include <iostream>
#include "../../unp.h"
#include <sys/un.h>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>

//处理僵死进程的可移植方法：捕获SIGCHLD信号，并调用wait或者waitpid函数
void sig_chld(int signo){
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0){
        std::cout << "child " << pid << " terminated\n";    //在实际的项目中，不要调用cout。此处只是为了演示。
    }
    return;
}

int main() {
    int listenfd, connfd;
    pid_t   childpid;
    socklen_t clilen;
    struct sockaddr_un cliaddr, servaddr;
    void sig_chld(int);
    listenfd = Socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(UNIXSTR_PATH);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sun_family= AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);
    Bind(listenfd, (sockaddr*)&servaddr, sizeof servaddr);
    Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_chld);
    for (;;) {
        clilen = sizeof cliaddr;
        if ((connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0){
            if (errno == EINTR){
                continue;
            }
            else{
                err_sys("accept error");
            }
        }
        if ((childpid = Fork()) == 0){
            ///child process
            Close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        Close(connfd);  ///parent closes connected socket
    }
    return 0;
}
