#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include "../../unp.h"
#include "mysdr.h"

#define SAP_NAME "sap.mcast.net" ///default group name and port
#define SAP_PORT "9875"

void loop(int sockfd, socklen_t salen){
    socklen_t len;
    ssize_t n;
    char *p;
    struct sockaddr *sa;
    struct sap_packet{
        uint32_t sap_header;    /// sap header
        uint32_t sap_src;       /// source address
        char sap_data[BUFFSIZE]; /// real data
    } buf;
    sa = static_cast<sockaddr *>(Malloc(salen));
    for(;;){
        len = salen;
        n = Recvfrom(sockfd, &buf, sizeof(buf) -1, 0, sa, &len);
        ((char*)&buf)[n] = 0;
        buf.sap_header = ntohl(buf.sap_header);
        printf("From %s hash 0x%04x\n", Sock_ntop(sa, len), buf.sap_header&SAP_HASH_MASK);
        if (((buf.sap_header&SAP_VERSION_MASK) >> SAP_VERSION_SHIFT) > 1){
            err_msg("... version field not 1 (0x%08x)", buf.sap_header);
            continue;
        }
        if (buf.sap_header & SAP_IPV6){
            err_msg("...IPv6");
            continue;
        }
        if (buf.sap_header & (SAP_DELETE|SAP_ENCRYPTED|SAP_COMPRESSED)){
            err_msg("... can't parse this packet type (0x%08x)", buf.sap_header);
            continue;
        }
        p = buf.sap_data + ((buf.sap_header & SAP_AUTHLEN_MASK) >> SAP_AUTHLEN_SHIFT);
        if (strcmp(p, "application/sdp") == 0){
            p += 16;
        }
        printf("%s\n", p);
    }
}

int main(int argc, char** argv) {
    std::cout << "Hello, World!" << std::endl;
    int sockfd;
    const int on = 1;
    socklen_t salen;
    struct sockaddr *sa;
    if (argc == 1){
        sockfd = Udp_client(SAP_NAME, SAP_PORT, &sa, &salen);
    }
    else if (argc == 4){
        sockfd = Udp_client(argv[1], argv[2], &sa, &salen);
    } else{
        err_quit("usage: mysdr <mcast-addr> <port#> <interface-name>");
    }
    Setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    Bind(sockfd, sa, salen);
    Mcast_join(sockfd, sa, salen, (argc == 4) ? argv[3]: nullptr, 0);
    loop(sockfd, salen);
    return 0;
}
