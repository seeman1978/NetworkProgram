#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/wait.h>
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
    int listenfd, connfd, udpfd, nready, maxfdp1;
    char mesg[MAXLINE];
    pid_t childpid;
    fd_set rset;
    uint32_t n;
    socklen_t len;
    const int on = 1;
    struct sockaddr_in cliaddr{}, servaddr{};

    ///create tcp socket
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);

    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    Listen(listenfd, 100);

    ///create udp socket
    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);
    Bind(udpfd, (sockaddr*)&servaddr, sizeof(servaddr));

    Signal(SIGCHLD, sig_chld);  //处理僵死进程

    //macos系统中，由于内核将重启被中断的系统调用，于是accept不会返回错误。
    FD_ZERO(&rset);
    maxfdp1 = std::max(listenfd, udpfd) + 1;
    for (;  ; ) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        if ((nready = select(maxfdp1, &rset, nullptr, nullptr, nullptr)) < 0){
            if (errno == EINTR){
                continue;   ///back to for()
            }
            else{
                err_sys("select error");
            }
        }
        if (FD_ISSET(listenfd, &rset)){
            len = sizeof(cliaddr);
            connfd = Accept(listenfd, (sockaddr*)&cliaddr, &len);
            if ((childpid = Fork()) == 0){///child process
                Close(listenfd);
                str_echo(connfd);
                exit(0);
            }
            Close(connfd);  ///parent closes connected socket
        }
        if (FD_ISSET(udpfd, &rset)){
            len = sizeof (cliaddr);
            n = Recvfrom(udpfd, mesg, MAXLINE, 0, (sockaddr*)&cliaddr, &len);
            Sendto(udpfd, mesg, n, 0, (sockaddr*)&cliaddr, len);
        }
    }
    return 0;
}
