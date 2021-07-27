#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <array>
#include "../../unp.h"


int main() {
    int i{0}, maxi{0}, maxfd{0}, listenfd{0}, connfd{0}, sockfd{0}, nready{0};
    int client[FD_SETSIZE];
    size_t n;
    fd_set rset, allset;
    char buf[MAXLINE];
    socklen_t  clilen;
    struct sockaddr_in cliaddr{}, servaddr{};

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    Listen(listenfd, 100);

    maxfd = listenfd;
    maxi = -1;  ///index into client[] array
    for (auto& c : client) {
        c = -1;
    }
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    //macos系统中，由于内核将重启被中断的系统调用，于是accept不会返回错误。
    for (;  ; ) {
        rset = allset;
        nready = Select(maxfd+1, &rset, nullptr, nullptr, nullptr);
        if (FD_ISSET(listenfd, &rset)){///new client connection
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (sockaddr*)&cliaddr, &clilen);
            i = 0;
            for (auto& c : client) {
                if (c < 0){
                    c = connfd; ///save descriptor
                    break;
                }
                ++i;
            }
            if (i == FD_SETSIZE){
                err_quit("too many clients");
            }
            FD_SET(connfd, &allset);    ///add a new descriptor to set
            if (connfd > maxfd){
                maxfd = connfd;
            }
            if (i > maxi){
                maxi = i;
            }
            if (--nready <= 0){
                continue;
            }
        }
        for (int j = 0; j <= maxi; ++j) {
            if ((sockfd=client[j]) < 0){
                continue;
            }
            if (FD_ISSET(sockfd, &rset)){
                if ((n= Read(sockfd, buf, MAXLINE)) == 0){///connection closed by client
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[j] = -1;
                }
                else{
                    Writen(sockfd, buf, n);
                }
                if (--nready <= 0){///no more readable descriptors
                    break;
                }
            }
        }
    }
}
