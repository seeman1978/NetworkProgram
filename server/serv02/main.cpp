#include <cstdlib>
#include <unistd.h>
#include <wait.h>
#include <cerrno>
#include "../../unp.h"

static int nchildren;
static pid_t *pids;

void sig_int(int signo){
    void pr_cpu_time();
    for (int i = 0; i < nchildren; ++i) {
        kill(pids[i], SIGTERM);
    }
    while (wait(nullptr) > 0){
        ;   /// wait for all children
    }
    if (errno != ECHILD){
        err_sys("wait error");
    }
    pr_cpu_time();
    exit(0);
}

int main(int argc, char** argv) {
    int listenfd, i;
    socklen_t addrlen;

    pid_t child_make(int, int, int);
    if (argc == 3){
        listenfd = Tcp_listen(nullptr, argv[1], &addrlen);
    }
    else if (argc == 4){
        listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    }
    else{
        err_quit("usage : serv02 [<host>] <port#> <children#>");
    }
    nchildren = atoi(argv[argc-1]);
    pids = static_cast<pid_t *>(Calloc(nchildren, sizeof(pid_t)));
    for (i = 0; i < nchildren; ++i) {
        pids[i] = child_make(i, listenfd, addrlen);
    }
    Signal(SIGINT, sig_int);
    for (;;){
        pause(); /// everything done by children
    }
    return 0;
}
