#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <syslog.h>		/* for syslog() */
#include "../../unp.h"
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
    pid_t childpid;
    socklen_t chilen;
    struct sockaddr_in cliaddr{}, servaddr{};

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 100);
    Signal(SIGCHLD, sig_chld);  //处理僵死进程
    //macos系统中，由于内核将重启被中断的系统调用，于是accept不会返回错误。
    for (;  ; ) {
        chilen = sizeof(cliaddr);
        connfd  = accept(listenfd, (sockaddr*)&cliaddr, &chilen);
        if(connfd < 0){
            if(errno == EINTR){
                continue;
            }
            else{
                err_sys("accept error");    //打印错误消息，并退出程序
            }
        }
        str_echo(connfd);
        if ((childpid = fork()) < 0) {
            err_sys("fork failed");
        } else if (childpid == 0) {	/* child */
            close(listenfd);
            str_echo(connfd);
            exit(0);
        } else {		/* parent */
            close(connfd);
        }
    }
    return 0;
}
