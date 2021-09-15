//
// Created by qiangwang on 2021/9/15.
//
#include <cstdlib>
#include "../unp.h"

#define MAXN 16384 /// MAX #BYTES client can request

void web_child(int sockfd){
    int ntowrite;
    ssize_t nread;
    char line[MAXLINE], result[MAXLINE];
    for (; ;) {
        if ((nread = Readline(sockfd, line, MAXLINE)) == 0){
            return; /// connection closed by other end
        }
        ntowrite = atol(line);
        if ((ntowrite <= 0 || ntowrite > MAXN)){
            err_quit("client request for %d bytes", ntowrite);
        }
        Writen(sockfd, result, ntowrite);
    }
}