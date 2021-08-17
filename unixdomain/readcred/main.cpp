#include <sys/un.h>
#include "../../unp.h"
int main() {
    int sockfd;
    struct sockaddr_un servaddr;
    sockfd = Socket(AF_LOCAL, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);
    Connect(sockfd, (sockaddr*)&servaddr, sizeof servaddr);
    str_echo(sockfd);
    return 0;
}
