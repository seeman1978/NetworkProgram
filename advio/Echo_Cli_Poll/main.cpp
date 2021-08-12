#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>

#include "../../unp.h"

void str_cli(FILE* fp, int sockfd){
    int stdineof, n, wfd;
    char buf[MAXLINE];
    struct pollfd arrPollfd[2];
    struct dvpoll dopoll;
}

int main() {
    int sockfd{0};

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

    str_cli(stdin, sockfd);

    return 0;
}

