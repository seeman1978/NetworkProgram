#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>

#define MAXLINE 4096
int main() {
    std::cout << "Hello, World!" << std::endl;
    int sockfd{0}, n{0};
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr{};
    //ip v4 tcp
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "socket error " << sockfd << '\n';
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1313);
    if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0){
        std::cout << "inet_pton error for " << "127.0.0.1" << '\n';
    }
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        //if server does not bind or listen, connect will fail.
        std::cout << "connect error" << '\n';
        return 0;
    }
    while ((n=read(sockfd, recvline, MAXLINE)) > 0){
        recvline[n] = 0;
        std::cout << recvline << '\n';
    }
    if (n < 0){
        std::cout << "read error\n";
    }

    return 0;
}
