#include <iostream>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include "../../unp.h"

static sigjum_buf jmpbuf;

static void recvfrom_alarm(int signo){
    siglongjmp(jmpbuf, 1);
}

void dg_cli(FILE *fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen){
    int n;
    const int on = 1;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    socklen_t len;
    struct sockaddr * preply_addr;

    preply_addr = static_cast<sockaddr *>(Malloc(servlen));

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    Signal(SIGALRM, recvfrom_alarm);

    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(5);
        for (; ; ) {
            if (sigsetjmp(jmpbuf, 1) != 0){
                break;
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