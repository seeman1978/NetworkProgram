#include <ctime>
#include <cstring>
#include "../../unp.h"
int main(int argc, char** argv) {
    int sockfd;
    ssize_t n;
    char buff[MAXLINE];

    time_t ticks;
    socklen_t len;
    struct sockaddr_storage cliaddr{};

    if (argc == 2){
        sockfd = Udp_server(nullptr, argv[1], nullptr);
    }
    else if (argc == 3){
        sockfd = Udp_server(argv[1], argv[2], nullptr);
    }
    else{
        err_quit("usage: udp server1 [<host>] <service or port#>");
    }

    for (;;){
        len = sizeof cliaddr;
        n = Recvfrom(sockfd, buff, MAXLINE, 0, (sockaddr*)&cliaddr, &len);
        printf("datagram from %s\n", Sock_ntop((sockaddr*)&cliaddr, len));

        ticks = time(nullptr);
        snprintf(buff, sizeof buff, "%.24s\r\n", ctime(&ticks));
        Sendto(sockfd, buff, strlen(buff), 0, (sockaddr*)&cliaddr, len);
    }
    return 0;
}
