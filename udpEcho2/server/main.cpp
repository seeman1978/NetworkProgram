#include <iostream>
#include <netinet/in.h>
#include "../../unp.h"

void dg_echo(int sockfd, sockaddr* pCliAddr, socklen_t clilen){
    uint32_t n{0};
    socklen_t len{0};
    char msg[MAXLINE];
    for (;;){
        len = clilen;
        n = Recvfrom(sockfd, msg, MAXLINE, 0, pCliAddr, &clilen);
        Sendto(sockfd, msg, n, 0, pCliAddr, len);
    }
}

int main() {
    int sockfd{0};
    struct sockaddr_in servaddr{}, cliaddr{};
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1234);
    Bind(sockfd, (sockaddr*)&servaddr, sizeof(cliaddr));
    dg_echo(sockfd, (sockaddr*)&cliaddr, sizeof(cliaddr));
    return 0;
}
