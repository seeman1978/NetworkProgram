#include "../../unp.h"

int main(int argc, char** argv) {
    int listenfd, connfd, n;
    char buff[100];
    fd_set rset, xset;
    if (argc == 2){
        listenfd = Tcp_listen(nullptr, argv[1], nullptr);
    }
    else if (argc == 3){
        listenfd = Tcp_listen(argv[1], argv[2], nullptr);
    }
    else{
        err_quit("usage: tcprecv02 [<host> ] <port#>");
    }
    connfd = Accept(listenfd, nullptr, nullptr);
    FD_ZERO(&rset);
    FD_ZERO(&xset);
    for(;;){
        FD_SET(connfd, &rset);
        FD_SET(connfd, &xset);
        Select(connfd+1, &rset, nullptr, &xset, nullptr);
        if (FD_ISSET(connfd, &xset)){
            n = Recv(connfd, buff, sizeof(buff)-1, MSG_OOB);
            buff[n] = 0;
            printf("read %d OOB byte :%s\n", n, buff);
        }
        if (FD_ISSET(connfd, &rset)){
            if ((n= Read(connfd, buff, sizeof(buff)-1)) == 0){
                printf("received EOF\n");
                break;
            }
            buff[n] = 0;
            printf("read %d bytes:%s\n", n, buff);
        }
    }
    return 0;
}
