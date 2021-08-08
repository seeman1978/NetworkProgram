#include <ctime>
#include <cstring>
#include "../../unp.h"
int main(int argc, char** argv) {
    int listenfd{0}, connfd;
    socklen_t len, addrlen;
    char buff[MAXLINE];
    time_t ticks;
    struct sockaddr_storage cliaddr{};

    if (argc == 2){
        listenfd = Tcp_listen(nullptr, argv[1], &addrlen);
    }
    else if (argc == 3){
        listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    }
    else{
        err_quit("usage: tcp listen server2 [<host>] <service or port#>");
    }

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
