#include <iostream>
#include "../../libroute/unproute.h"
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>

#define BUFLEN (sizeof(struct rt_msghdr) + 512)
/// sizeof(struct sockaddr_in6)*8 = 192
#define SEQ 9999
#define RTAX_MAX	11	/* size of array to allocate */
int main(int argc, char** argv)
{
    int sockfd;
    char *buf;
    pid_t pid;
    ssize_t n;
    struct rt_msghdr *rtm;
    struct sockaddr *sa, *rti_info[RTAX_MAX];
    struct sockaddr_in *sin;
    if (argc != 2){
        err_quit("usage: getrt <IPaddress>");
    }
    sockfd = Socket(AF_ROUTE, SOCK_RAW, 0); /// need super user privileges
    buf = Calloc(1, BUFLEN);
    return 0;
}
