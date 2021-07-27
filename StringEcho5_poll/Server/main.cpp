#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <poll.h>
#include "../../unp.h"


int main() {
    int i{0}, maxi{0}, maxfd{0}, listenfd{0}, connfd{0}, sockfd{0}, nready{0};
    size_t n{0};
    char buf[MAXLINE];
    socklen_t  clilen;
    struct pollfd client[FOPEN_MAX];
    struct sockaddr_in cliaddr{}, servaddr{};

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    Listen(listenfd, 100);

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for (auto &c:client) {
        c.fd = -1;
    }
    maxi = 0;

    //macos系统中，由于内核将重启被中断的系统调用，于是accept不会返回错误。
    for (;  ; ) {
        nready = Poll(client, maxi + 1, -1);    ///(-1)    /* infinite poll timeout */
        if (client[0].revents & POLLRDNORM){///new client connection
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (sockaddr*)&cliaddr, &clilen);
            for (i=1; i<FOPEN_MAX; ++i) {
                if (client[i].fd < 0){
                    client[i].fd = connfd;
                    break;
                }
            }
            if (i == FOPEN_MAX){
                err_quit("too many clients");
            }
            client[i].events = POLLRDNORM;
            if (i > maxi){
                maxi = i;
            }
            if(--nready <= 0){
                continue;
            }
        }
        for (i = 1; i <= maxi ; ++i) {
            if((sockfd = client[i].fd) < 0){
                continue;
            }
            if (client[i].revents & (POLLRDNORM|POLLERR)){
                if ((n= read(sockfd, buf, MAXLINE)) < 0){
                    if (errno == ECONNRESET){///connection reset by client
                        Close(sockfd);
                        client[i].fd = -1;
                    }
                    else{
                        err_sys("read error");
                    }
                }
                else if (n == 0){///connection closed by client
                    Close(sockfd);
                    client[i].fd = -1;
                }
                else{
                    Writen(sockfd, buf, n);
                }
                if (--nready <= 0){///no more readable descriptor
                    break;
                }
            }
        }
    }
}
