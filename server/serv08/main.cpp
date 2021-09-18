#include <cstdlib>
#include <wait.h>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include "../../unp.h"
typedef struct{
    pthread_t thread_tid;
    long thread_count;
} Thread;

constexpr auto MAXNCLI{32};

void sig_int(int signo){
    void pr_cpu_time();
    pr_cpu_time();
    exit(0);
}

int main(int argc, char** argv) {
    int clifd[MAXNCLI], iget{0}, iput{0};
    int listenfd, connfd;
    std::mutex m;
    std::condition_variable cv;
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
    cliaddr = static_cast<sockaddr *>(Malloc(addrlen));

    int nthreads = atoi(argv[argc-1]);
    auto *tptr = static_cast<Thread *>(Calloc(nthreads, sizeof(Thread)));

    for (int i = 0; i < nthreads; ++i) {
        std::thread([&](){
            int connfd;
            void web_clild(int);
            std::cout << "thread " << i  << " starting\n";
            for (;;){
                std::unique_lock<std::mutex> lock{m};
                while (iget == iput){
                    cv.wait(lock);
                }
                connfd = clifd[iget];
                if (++iget == MAXNCLI){
                    iget = 0;
                }
                lock.unlock();

                ++tptr[i].thread_count;
                web_child(connfd);
                Close(connfd);
            }
        }).detach();
    }

    Signal(SIGINT, sig_int);
    for (;;){
        clilen = addrlen;
        connfd = Accept(listenfd, cliaddr, &clilen);
        std::lock_guard<std::mutex> lock{m};
        clifd[iput++] = connfd;
        if (iput == MAXNCLI){
            iput = 0;
        }
        if (iput == iget){
            err_quit("iput == iget = %d", iput);
        }
        cv.notify_one();
    }
    return 0;
}