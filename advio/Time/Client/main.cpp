#include <iostream>
#include <sys/socket.h>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include "../../../unp.h"
static void sig_alrm(int signo){
    return; ///just interrupt the recvfrom()
}

void dg_cli(FILE *fp, int sockfd, const sockaddr* pservaddr, socklen_t servlen){
    int n;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    Signal(SIGALRM, sig_alrm);
    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        alarm(5);
        if ((n= recvfrom(sockfd, recvline, MAXLINE, 0, nullptr, nullptr)) < 0){
            if (errno == EINTR){
                fprintf(stderr, "socker timeout\n");
            }
            else{
                err_sys("recvfrom error");
            }
        }
        else{
            alarm(0);
            recvline[n] = 0;
            Fputs(recvline, stdout);
        }
    }
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
