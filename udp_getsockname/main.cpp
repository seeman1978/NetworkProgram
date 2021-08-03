#include <iostream>
#include <netinet/in.h>
#include "../unp.h"
int main() {
    int sockfd;
    socklen_t len;
    struct sockaddr_in cliaddr, servaddr;

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1234);
    Inet_pton(AF_INET, "www.sina.com.cn", &servaddr.sin_addr);
    Connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    len = sizeof(cliaddr);
    Getsockname(sockfd, (sockaddr*)&cliaddr, &len);
    std::cout << "local address " << Sock_ntop((sockaddr*)&cliaddr, len) << '\n';
    return 0;
}
