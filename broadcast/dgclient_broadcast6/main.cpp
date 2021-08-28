#include <iostream>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include "../../unp.h"

static int pipefd[2];

static void recvfrom_alarm(int signo){
    char ch[2]{""};
    Write(pipefd[1], ch, 1);    /// write one null byte to pipe
    return;
}

void dg_cli(FILE *fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen){
    int n, maxfdp1;
    const int on = 1;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    fd_set rset;
    socklen_t len;
    struct sockaddr * preply_addr;

    preply_addr = static_cast<sockaddr *>(Malloc(servlen));

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    Pipe(pipefd);
    maxfdp1 = std::max(sockfd, pipefd[0]) + 1;

    FD_ZERO(&rset);

    Signal(SIGALRM, recvfrom_alarm);

    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(5);
        for (; ; ) {
            FD_SET(sockfd, &rset);
            FD_SET(pipefd[0], &rset);
            if ((n = select(maxfdp1, &rset, nullptr, nullptr, nullptr)) < 0){
                if (errno == EINTR){
                    continue;
                }
                else{
                    err_sys("select error");
                }
            }
            if (FD_ISSET(sockfd, &rset)){
                len = servlen;
                n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
                recvline[n] = 0;
                printf("from %s:%s", Sock_ntop_host(preply_addr, len), recvline);
            }
            if (FD_ISSET(pipefd[0], &rset)){
                Read(pipefd[0], &n, 1); /// timer expired
                break;
            }
        }
    }
    free(preply_addr);
}

int main() {
    int sockfd{0};
    struct sockaddr_in servaddr{} ;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1234);
    Inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    dg_cli(stdin, sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    return 0;
}