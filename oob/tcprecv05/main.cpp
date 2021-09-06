#include <csignal>
#include <sys/fcntl.h>
#include <unistd.h>
#include "../../unp.h"

int listenfd, connfd;
void sig_urg(int signo){
    int n;
    char buff[2048];
    printf("SIGURG received\n");
    n = Recv(connfd, buff, sizeof(buff)-1, MSG_OOB);
    buff[n] = 0;
    printf("read %d OOB bytes:%s\n", n, buff);
}

int main(int argc, char** argv) {
    if (argc == 2){
        listenfd = Tcp_listen(nullptr, argv[1], nullptr);
    }
    else if (argc == 3){
        listenfd = Tcp_listen(argv[1], argv[2], nullptr);
    }
    else{
        err_quit("usage: tcprecv01 [<host> ] <port#>");
    }

    int size{4096};
    Setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof size);

    connfd = Accept(listenfd, nullptr, nullptr);
    Signal(SIGURG, sig_urg);
    Fcntl(connfd, F_SETOWN, getpid());
    for(;;){
        pause();
    }
    return 0;
}
