#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

char *
sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];		/* Unix domain is largest */

    switch (sa->sa_family) {
        case AF_INET: {
            struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

            if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                return(NULL);
            return(str);
        }
        default:
            snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d",
                     sa->sa_family, salen);
            return(str);
    }
}

void dg_cli(FILE *fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen){
    int n, maxfdp1;
    const int on = 1;
    char sendline[1024]{"world"}, recvline[1024];

    socklen_t len;
    struct sockaddr * preply_addr;

    preply_addr = static_cast<sockaddr *>(malloc(servlen));

    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    len = servlen;
    n = recvfrom(sockfd, recvline, 1024, 0, preply_addr, &len);
    recvline[n] = 0;
    printf("from %s:%s\n", sock_ntop_host(preply_addr, len), recvline);

    struct sockaddr_in recvaddr{};
    bzero(&recvaddr, sizeof(recvaddr));
    recvaddr.sin_family = AF_INET;
    recvaddr.sin_addr.s_addr = INADDR_BROADCAST;
    recvaddr.sin_port = htons(1234);
    sendto(sockfd, sendline, strlen(sendline), 0, (sockaddr*)&recvaddr, sizeof(recvaddr));

    free(preply_addr);
}

int main() {
    int sockfd{0};
    struct sockaddr_in servaddr{} ;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(4321);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    dg_cli(stdin, sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    return 0;
}
