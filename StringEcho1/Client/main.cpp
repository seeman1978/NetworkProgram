#include <iostream>
#include <cstring>
#include <sys/syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../unp.h"

int main() {
    int sockfd[5]{0};

    struct sockaddr_in servaddr{};
    for (int j = 0; j < ; ++j) {
        sockfd[j] = socket(AF_INET, SOCK_STREAM, 0);
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
    }
    str_cli(stdin, sockfd[2]);

    return 0;
}

