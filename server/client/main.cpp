#include <cstdlib>
#include <cstring>
#include <wait.h>
#include <cerrno>
#include "../../unp.h"

constexpr auto MAXN = 16384;

int main(int argc, char** argv) {
    int i, j, fd;
    long nchildren, nloops, nbytes;
    pid_t pid;
    ssize_t n;
    char request[MAXLINE], reply[MAXN];

    if (argc != 6){
        err_quit("usage:client <hostname or IPaddr> <port> <#children>"
                 "<#loops/child> <#bytes/request>");
    }
    char* p_end{nullptr};
    nchildren = std::strtol(argv[3], &p_end, 10);
    nloops = std::strtol(argv[4], &p_end, 10);
    nbytes = std::strtol(argv[5], &p_end, 10);
    snprintf(request, sizeof(request), "%ld\n", nbytes);
    for (i = 0; i < nchildren; ++i) {
        if ((pid = Fork()) == 0){   ///child
            for (j = 0; j < nloops; ++j) {
                fd = Tcp_connect(argv[1], argv[2]);
                Write(fd, request, strlen(request));
                if ((n = Readn(fd, reply, nbytes)) != nbytes){
                    err_quit("server returned %d bytes", n);
                }
                Close(fd);
            }
            printf("child %d done\n", i);
            exit(0);
        }
    }

    while (wait(nullptr) > 0){
        ;///now parent waits for all children
    }
    if (errno != ECHILD){
        err_sys("wait error");
    }
    return 0;
}
