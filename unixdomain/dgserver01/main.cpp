#include <iostream>
#include <sys/un.h>
#include <unistd.h>
#include "../../unp.h"
int main() {
    int sockfd;
    struct sockaddr_un servaddr, cliaddr;
    sockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);
    unlink(UNIXDG_PATH);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXDG_PATH);
    Bind(sockfd, (sockaddr*)&servaddr, sizeof servaddr);
    dg_echo(sockfd, (sockaddr*)&cliaddr, sizeof cliaddr);
    return 0;
}
