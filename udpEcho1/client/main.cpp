#include <iostream>
#include <netinet/in.h>
#include "../../unp.h"
void dg_cli(FILE *fp, int sockfd, const sockaddr* pServAddr, socklen_t servlen){
    uint32_t n{0};
    char sendline[MAXLINE], recvline[MAXLINE+1];
    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        Sendto(sockfd, sendline, strlen(sendline), 0, pServAddr, servlen);
        n = Recvfrom(sockfd, recvline, MAXLINE, 0, nullptr, nullptr);
        recvline[n] = 0;
        Fputs(recvline, stdout);
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
