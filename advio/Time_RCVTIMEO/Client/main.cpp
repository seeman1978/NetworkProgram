#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include "../../../unp.h"

void dg_cli(FILE *fp, int sockfd, const sockaddr* pservaddr, socklen_t servlen){
    int n{0}; //negative is error;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    Setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        n = recvfrom(sockfd, recvline, MAXLINE, 0, nullptr, nullptr);
        if (n<0){
            if (errno == EWOULDBLOCK){
                fprintf(stderr, "socket timeout\n");
                continue;
            }
            else{
                err_sys("recvfrom error");
            }
        }
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}

int main() {
    int sockfd{0};
    struct sockaddr_in servaddr{} ;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1234);
    Inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    dg_cli(stdin, sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    return 0;
}
