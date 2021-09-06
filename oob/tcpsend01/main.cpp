#include <unistd.h>

#include "../../unprtt.h"
int main(int argc, char** argv) {
    int sockfd;
    if (argc != 3){
        err_quit("usage: tcpsend01 <host> <port#>");
    }
    sockfd = Tcp_connect(argv[1], argv[2]);
    Write(sockfd, (void *) "123", 3);
    printf("wrote 3 bytes of normal data\n");
    sleep(1);

    Send(sockfd, "4", 1, MSG_OOB);
    printf("worte 1 byte of OOB data\n");
    sleep(1);

    Write(sockfd, (void *)"56", 2);
    printf("wrote 2 bytes of normal data\n");
    sleep(1);

    Send(sockfd, "7", 1, MSG_OOB);
    printf("wrote 1 byte of OOB data\n");
    sleep(1);

    Write(sockfd, (void *)"89", 2);
    printf("wrote 2 bytes of normal data\n");
    sleep(1);
    return 0;
}
