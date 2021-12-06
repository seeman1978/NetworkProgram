#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

void dg_serv(int sockfd){
    const int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    std::string str{"hello"};

    struct sockaddr_in recvaddr{};
    bzero(&recvaddr, sizeof(recvaddr));
    recvaddr.sin_family = AF_INET;
    recvaddr.sin_addr.s_addr = INADDR_BROADCAST;
    recvaddr.sin_port = htons(4321);

    sendto(sockfd, str.c_str(), str.size(), 0, (sockaddr*)&recvaddr, sizeof(recvaddr));

    char recvline[1024];
    socklen_t len = sizeof(recvaddr);
    struct sockaddr * preply_addr;
    preply_addr = static_cast<sockaddr *>(malloc(len));
    ssize_t n = recvfrom(sockfd, recvline, 1023, 0, preply_addr, &len);
    std::cout << "received: " << recvline << ".length is " << n << '\n';
    free(preply_addr);
}

int main() {
    int sockfd{0};
    struct sockaddr_in servaddr{};
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(1234);
    bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    //dg_echo(sockfd, (sockaddr*)&cliaddr, sizeof(cliaddr));
    dg_serv(sockfd);
    return 0;

}
