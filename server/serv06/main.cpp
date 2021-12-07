#include <cerrno>
#include <cstdlib>
#include <wait.h>
#include <thread>
#include "../../unp.h"

void sig_int(int signo){
    void pr_cpu_time();
    pr_cpu_time();
    exit(0);
}

int main(int argc, char** argv) {
    int listenfd, connfd;

    void sig_int(int), web_child(int);
    socklen_t clilen, addrlen;

    struct sockaddr* cliaddr;

    if (argc == 2){
        listenfd = Tcp_listen(nullptr, argv[1], &addrlen);
    }
    else if (argc == 3){
        listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    }
    else{
        err_quit("usage:serv06 [<host>] <port#>");
    }

    cliaddr = static_cast<sockaddr *>(Malloc(addrlen));
    Signal(SIGINT, sig_int);
    for (;;){
        clilen = addrlen;
        connfd = accept(listenfd, cliaddr, &clilen);
        std::thread([=](){
            web_child(connfd);
            Close(connfd);
        }).detach();
    }
    return 0;
}
