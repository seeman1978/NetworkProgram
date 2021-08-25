#include "../unpifi.h"
#include <net/if_arp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/ioctl.h>

int main() {
    int sockfd{0};
    struct ifi_info *ifi{nullptr};
    char *ptr{nullptr};
    struct arpreq arpreq;
    struct sockaddr_in *sin{nullptr};
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    for (ifi = get_ifi_info(AF_INET, 0); ifi != nullptr; ifi = ifi->ifi_next) {
        printf("%s: ", Sock_ntop(ifi->ifi_addr, sizeof(struct sockaddr_in)));
        sin = (struct sockaddr_in*)&arpreq.arp_pa;
        memcpy(sin, ifi->ifi_addr, sizeof(struct sockaddr_in));
        if (ioctl(sockfd, SIOCGARP, &arpreq) < 0){
            err_ret("ioctl SIOCGARP");
            continue;
        }
        ptr = &arpreq.arp_ha.sa_data[0];
        printf("%x:%x:%x:%x:%x:%x\n", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
    }

    return 0;

}
