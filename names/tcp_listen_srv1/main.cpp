#include <ctime>
#include <cstring>
#include "../../unp.h"
int main(int argc, char** argv) {
    int listenfd, connfd;
    socklen_t len;
    char buff[MAXLINE];
    time_t ticks;
    struct sockaddr_storage cliaddr{};

    if (argc != 2){
        err_quit("usage: tcp listen server1 <service or port#>");
    }
    listenfd = Tcp_listen(nullptr, argv[1], nullptr);
    for (;;){
        len = sizeof cliaddr;
        connfd = Accept(listenfd, (sockaddr*)&cliaddr, &len);
        printf("connect from %s\n", Sock_ntop((sockaddr*)&cliaddr, len));

        ticks = time(nullptr);
        snprintf(buff, sizeof buff, "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));
        Close(connfd);
    }
    return 0;
}
