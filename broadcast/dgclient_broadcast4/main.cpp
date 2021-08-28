#include <iostream>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include "../../unp.h"

static void recvfrom_alarm(int signo){
    return;
}

void dg_cli(FILE *fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen){
    int n;
    const int on = 1;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    fd_set rset;
    sigset_t sigset_alrm, sigset_empty;
    socklen_t len;
    struct sockaddr * preply_addr;
    preply_addr = static_cast<sockaddr *>(Malloc(servlen));
    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    FD_ZERO(&rset);

    Sigemptyset(&sigset_empty);
    Sigemptyset(&sigset_alrm);
    Sigaddset(&sigset_alrm, SIGALRM);
    Signal(SIGALRM, recvfrom_alarm);

    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        Sigprocmask(SIG_BLOCK, &sigset_alrm, nullptr);
        alarm(5);
        for (; ; ) {
            FD_SET(sockfd, &rset);
            n = pselect(sockfd+1, &rset, nullptr, nullptr, nullptr, &sigset_empty);
            if (n < 0){
                if (errno == EINTR){
                    break;
                }
                else{
                    err_sys("pselect error: returned %d", n);
                }
            }
            else if (n != 1){
                err_sys("pselect error: returned %d", n);
            }
            len = servlen;
            n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
            recvline[n] = 0;
            printf("from %s:%s", Sock_ntop_host(preply_addr, len), recvline);
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