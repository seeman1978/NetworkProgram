#include <iostream>
#include <cstring>
#include "../../unp.h"
#include <sys/utsname.h>
#include <unistd.h>

#define SENDRATE 5
void send_all(int sendfd, struct sockaddr *sadest, socklen_t salen){
    char line[MAXLINE]; /// HOSTNAME ADN PROCESS ID
    struct utsname myname;
    if (uname(&myname) < 0){
        err_sys("uname error");
    }
    snprintf(line, sizeof line, "%s, %d\n", myname.nodename, getpid());
    for(;;){
        Sendto(sendfd, line, strlen(line), 0, sadest, salen);
        sleep(SENDRATE);
    }
}

void recv_all(int recvfd, socklen_t salen){
    int n;
    char line[MAXLINE];
    socklen_t len;
    struct sockaddr *safrom;
    safrom = static_cast<sockaddr *>(Malloc(salen));
    for(;;){
        len = salen;
        n = Recvfrom(recvfd, line, MAXLINE, 0, safrom, &len);
        line[n] = 0;
        printf("from %s:%s", Sock_ntop(safrom, len), line);
    }
}

int main(int argc, char** argv) {
    int sendfd, recvfd;
    const int on = 1;
    socklen_t salen;
    struct sockaddr *sasend, *sarecv;
    if (argc != 3){
        err_quit("usage: sendrecv <IP-multicast-address> <port#>");
    }
    sendfd = Udp_client(argv[1], argv[2], &sasend, &salen);
    recvfd = Socket(sasend->sa_family, SOCK_DGRAM, 0);
    Setsockopt(recvfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    sarecv = static_cast<sockaddr *>(Malloc(salen));
    memcpy(sarecv, sasend, salen);
    Bind(recvfd, sarecv, salen);
    Mcast_join(recvfd, sasend, salen, nullptr, 0);  /// add multi group
    Mcast_set_loop(sendfd, 0);  /// disable loop 禁止回馈
    if (Fork() == 0){
        /// child --> receives
        recv_all(recvfd, salen);
    }
    send_all(sendfd, sasend, salen); /// parent ---> sends
    return 0;
}
