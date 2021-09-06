#include <csignal>
#include <sys/fcntl.h>
#include <unistd.h>
#include "../../unp.h"

int listenfd, connfd;
void sig_urg(int signo){
    int n;
    char buff[100];
    printf("SIGURG received\n");
    n = Recv(connfd, buff, sizeof(buff)-1, MSG_OOB);
    buff[n] = 0;
    printf("read %d OOB bytes:%s\n", n, buff);
}

int main(int argc, char** argv) {
    int n;
    char buff[100];
    if (argc == 2){
        listenfd = Tcp_listen(nullptr, argv[1], nullptr);
    }
    else if (argc == 3){
        listenfd = Tcp_listen(argv[1], argv[2], nullptr);
    }
    else{
        err_quit("usage: tcprecv01 [<host> ] <port#>");
    }
    connfd = Accept(listenfd, nullptr, nullptr);
    Signal(SIGURG, sig_urg);
    Fcntl(connfd, F_SETOWN, getpid());
    for(;;){
        if ((n=Read(connfd, buff, sizeof(buff)-1)) == 0){
            printf("received EOF\n");
            break;
        }
        buff[n] = 0;
        printf("read %d bytes : %s\n", n, buff);
    }
    return 0;
}
