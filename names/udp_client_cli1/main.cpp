#include <iostream>
#include "../../unp.h"
int main(int argc, char** argv) {
    int sockfd;
    size_t n;
    char recvline[MAXLINE+1];
    socklen_t salen;
    struct sockaddr* sa;
    if (argc != 3){
        err_quit("usage: udp client cli1: <hostname/ip address> <service/port#>");
    }
    sockfd = Udp_client(argv[1], argv[2], &sa, &salen);
    printf("send to %s\n", Sock_ntop_host(sa, salen));
    Sendto(sockfd, " ", 1, 0, sa, salen);

    n = Recvfrom(sockfd, recvline, MAXLINE, 0, nullptr, nullptr);
    recvline[n] = '\0';
    Fputs(recvline, stdout);
    return 0;
}
