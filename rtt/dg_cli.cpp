//
// Created by qiangwang on 2021/9/2.
//

#include "../unp.h"

ssize_t Dg_send_recv(int, const void *, size_t, void *, size_t, const struct sockaddr*, socklen_t);

void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen){
    ssize_t n;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        n = Dg_send_recv(sockfd, sendline, strlen(sendline),
                         recvline, MAXLINE, pservaddr, servlen);
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}