//
// Created by qiangwang on 2021/8/8.
//

#include <cstdlib>
#include "../../unp.h"

int main(int argc, char **argv){
    int sockfd;
    size_t n;
    char recvline[MAXLINE+1];
    socklen_t len;
    struct sockaddr_storage ss{};
    if (argc != 3){
        err_quit("usage : daytimetcpcli <hostname/IPaddress> <service/port#>");
    }
    sockfd = Tcp_connect(argv[1], argv[2]);
    len = sizeof ss;
    Getpeername(sockfd, (sockaddr*)&ss, &len);
    printf("connect to %s\n", Sock_ntop_host((sockaddr*)&ss, len));
    while ((n= Read(sockfd, recvline, MAXLINE)) > 0){
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
    exit(0);
}
