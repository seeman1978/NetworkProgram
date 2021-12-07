#include <cerrno>
#include <cstdlib>
#include <wait.h>
#include "../../unp.h"

void sig_int(int signo){
    void pr_cpu_time();
    pr_cpu_time();
    exit(0);
}

//处理僵死进程的可移植方法：捕获SIGCHLD信号，并调用wait或者waitpid函数
void sig_chld(int signo){
    pid_t pid;
    int stat;
    pid = wait(&stat);
}

int main(int argc, char** argv) {
    int listenfd, connfd;
    pid_t childpid;
    void sig_chld(int), sig_int(int), web_child(int);
    socklen_t clilen, addrlen;
    struct sockaddr* cliaddr;
    if (argc == 2){
        listenfd = Tcp_listen(nullptr, argv[1], &addrlen);
    }
    else if (argc == 3){
        listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    }
    else{
        err_quit("usage:serv01 [<host>] <port#>");
    }

    cliaddr = static_cast<sockaddr *>(Malloc(addrlen));
    Signal(SIGCHLD, sig_chld);
    Signal(SIGINT, sig_int);
    for (;;){
        clilen = addrlen;
        if ((connfd = accept(listenfd, cliaddr, &clilen)) < 0){
            if (errno == EINTR){
                continue;
            }
            else{
                err_sys("accept errro");
            }
        }
        if ((childpid = Fork()) == 0){
            /// child process
            Close(listenfd);    /// close listening socket
            web_child(connfd);
            exit(0);
        }
        Close(connfd);  /// parent closes connected socket
    }
    return 0;
}
