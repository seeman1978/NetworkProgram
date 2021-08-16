#include <iostream>
#include <sys/un.h>
#include <unistd.h>
#include "../../unp.h"
int main() {
    int sockfd;
    struct sockaddr_un cliaddr, servaddr;
    sockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);
    bzero(&cliaddr, sizeof cliaddr);
    cliaddr.sun_family = AF_LOCAL;

    char fnTemplate[17] = "/tmp/temp.XXXXXX";
    fnTemplate[16] = '\0';
    int fd = mkstemp(fnTemplate);
    if (fd == -1){
        err_quit("create tmp file failed.");
    }

    strcpy(cliaddr.sun_path, fnTemplate);
    unlink(cliaddr.sun_path);
    Bind(sockfd, (sockaddr*)&cliaddr, sizeof cliaddr);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXDG_PATH);
    dg_cli(stdin, sockfd, (sockaddr*)&servaddr, sizeof servaddr);
    return 0;
}
