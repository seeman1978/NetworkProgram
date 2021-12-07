#include <cstdlib>
#include <wait.h>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include "../../unp.h"
typedef struct{
    pthread_t thread_tid;
    long thread_count;
} Thread;

void sig_int(int signo){
    void pr_cpu_time();
    pr_cpu_time();
    exit(0);
}

int main(int argc, char** argv) {
    int listenfd;
    std::mutex m;
    void web_child(int);
    socklen_t clilen, addrlen;

    struct sockaddr* cliaddr;

    if (argc == 3){
        listenfd = Tcp_listen(nullptr, argv[1], &addrlen);
    }
    else if (argc == 4){
        listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    }
    else{
        err_quit("usage:serv07 [<host>] <port#> <#threads>");
    }
    int nthreads = atoi(argv[argc-1]);
    auto *tptr = static_cast<Thread *>(Calloc(nthreads, sizeof(Thread)));
    for (int i = 0; i < nthreads; ++i) {
        std::thread([&](){
            int connfd;
            void web_clild(int);
            socklen_t clilen;
            struct sockaddr* cliaddr;
            cliaddr = static_cast<sockaddr *>(Malloc(addrlen));
            std::cout << "thread " << i  << " starting\n";
            for (;;){
                clilen = addrlen;
                {
                    std::lock_guard<std::mutex> lock{m};
                    connfd = Accept(listenfd, cliaddr, &clilen);
                }
                ++tptr[i].thread_count;
                web_child(connfd);
                Close(connfd);
            }
        }).detach();
    }

    Signal(SIGINT, sig_int);
    for (;;){
        pause();
    }
    return 0;
}