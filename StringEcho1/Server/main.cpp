#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <syslog.h>		/* for syslog() */
#include "../../unp.h"

int main() {
    int listenfd, connfd;
    pid_t childpid;
    socklen_t chilen;
    struct sockaddr_in cliaddr{}, servaddr{};

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 100);
    Signal(SIGCHLD, sig_chld);
    for (;  ; ) {
        chilen = sizeof(cliaddr);
        connfd  = accept(listenfd, (sockaddr*)&cliaddr, &chilen);
        str_echo(connfd);
        if ((childpid = fork()) < 0) {
            err_sys("fork failed");
        } else if (childpid == 0) {	/* child */
            close(listenfd);
            str_echo(connfd);
            exit(0);
        } else {		/* parent */
            close(connfd);
        }
    }
    return 0;
}
