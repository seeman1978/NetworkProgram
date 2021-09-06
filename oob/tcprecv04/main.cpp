#include <unistd.h>
#include "../../unp.h"

int main(int argc, char** argv) {
    int listenfd, connfd, n, on{1};
    char buff[100];

    if (argc == 2){
        listenfd = Tcp_listen(nullptr, argv[1], nullptr);
    }
    else if (argc == 3){
        listenfd = Tcp_listen(argv[1], argv[2], nullptr);
    }
    else{
        err_quit("usage: tcprecv02 [<host> ] <port#>");
    }

    Setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on, sizeof on);

    connfd = Accept(listenfd, nullptr, nullptr);
    sleep(5);
    for(;;){
        if (Sockatmark(connfd)){
            printf("ao OOB mark\n");
        }

        if ((n= Read(connfd, buff, sizeof(buff)-1)) == 0){
            printf("received EOF\n");
            break;
        }
        buff[n] = 0;
        printf("read %d bytes:%s\n", n, buff);
    }
    return 0;
}
