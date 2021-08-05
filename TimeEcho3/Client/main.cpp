#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../unp.h"

int main(int argc, char **argv) {
    int sockfd;
    size_t n;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr{};
    struct in_addr  **pptr, *inetaddrp[2], inetaddr{};
    struct hostent *hp;
    struct servent *sp;
    if (argc != 3){
        err_quit("usage: daytimetcpcli3 <hostname> <service>");
    }
    if ((hp = gethostbyname(argv[1])) == nullptr){
        if (inet_aton(argv[1], &inetaddr) == 0){
            err_quit("hostname error for %s: %s", argv[1], hstrerror(h_errno));
        }
        else{
            inetaddrp[0] = &inetaddr;
            inetaddrp[1] = nullptr;
            pptr = inetaddrp;
        }
    }
    else{
        pptr = (struct in_addr **)hp->h_addr_list;
    }
    if ((sp = getservbyname(argv[2], "tcp")) == nullptr){
        err_quit("getservbyname error for %s", argv[2]);
    }
    for (; *pptr != nullptr; ++pptr) {
        sockfd = Socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = sp->s_port;
        memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
        printf("trying %s\n", Sock_ntop((sockaddr*)&servaddr, sizeof(servaddr)));
        if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) == 0){
            //success
            break;
        }
        err_ret("connect error;");
        close(sockfd);
    }
    if (*pptr == nullptr){
        err_quit("unable to connect");
    }
    while ((n = Read(sockfd, recvline, MAXLINE)) > 0){
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
    return 0;
}
