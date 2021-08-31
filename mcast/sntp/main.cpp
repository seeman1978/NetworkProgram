#include <iostream>
#include <cstring>
#include "../../unp.h"
#include "sntp.h"
int main(int argc, char** argv) {
    int sockfd;
    char buf[MAXLINE];
    ssize_t n;
    socklen_t salen, len;
    struct ifi_info *ifi{nullptr};
    struct sockaddr *mcastsa{nullptr}, *wild{nullptr}, *from{nullptr};
    struct timeval now;
    if (argc != 2){
        err_quit("usage: ssntp <IPAddress>");
    }
    sockfd = Udp_client(argv[1], "ntp", &mcastsa, &salen);
    wild = static_cast<sockaddr *>(Malloc(salen));
    memcpy(wild, mcastsa, salen);
    sock_set_wild(wild, salen);
    Bind(sockfd, wild, salen);
#ifdef MCAST
    for(ifi = Get_ifi_info(mcastsa->sa_family, 1); ifi != nullptr; ifi = ifi->ifi_next){
        if(ifi->ifi_flags & IFF_MULTICAST){
            Mcast_join(sockfd, mcastsa, salen, ifi->ifi_name, 0);
            printf("joined %s on %s\n", Sock_ntop(mcastsa, salen), ifi->ifi_name);
        }
    }
#endif
    from = static_cast<sockaddr *>(Malloc(salen));
    for(;;){
        len = salen;
        n = Recvfrom(sockfd, buf, sizeof buf, 0, from, &len);
        Gettimeofday(&now, nullptr);
        sntp_proc(buf, n, &now);
    }
    return 0;
}
