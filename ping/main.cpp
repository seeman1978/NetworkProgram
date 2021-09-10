#include "ping.h"
#include "../unp.h"
#include <unistd.h>
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cerrno>
#include <cstring>

void sig_alrm(int signo){
    (*pr->fsend)();
    alarm(1);
}

struct proto proto_v4{proc_v4, send_v4, nullptr, nullptr, nullptr, 0, IPPROTO_ICMP};
#ifdef IPV6
struct proto proto_v6{proc_v6, send_v6, init_v6, nullptr, nullptr, 0, IPPROTO_ICMPV6};
#endif

void tv_sub(struct timeval *out, struct timeval *in){
    if ((out->tv_usec -= in->tv_usec) < 0){
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

void proc_v4(char* ptr, ssize_t len, struct  msghdr* msg, struct timeval *tvrecv){
    int hlen1, icmplen;
    double rtt;
    struct ip *ip;
    struct icmp *icmp;
    struct timeval *tvsend;

    ip = (struct ip*)ptr; /// start of IP header
    hlen1 = ip->ip_hl << 2; /// length of IP header
    if (ip->ip_p != IPPROTO_ICMP){
        return;
    }
    icmp = (struct icmp*)(ptr+hlen1);   ///start of ICMP header
    if ((icmplen = len-hlen1) < 8){ /// malformed packet
        return;
    }
    if (icmp->icmp_type == ICMP_ECHOREPLY){
        if (icmp->icmp_id != pid){
            return; /// not a response to our ECHO_REQUEST
        }
        if (icmplen < 16){
            return; /// not enough data to use
        }
        tvsend = (struct timeval*)icmp->icmp_data;
        tv_sub(tvrecv, tvsend);
        rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec/1000.0;
        printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n", icmplen, Sock_ntop_host(pr->sarecv, pr->salen),
               icmp->icmp_seq, ip->ip_ttl, rtt);
    }
    else if (verbose){
        printf("%d bytes from %s : type=%d, code=%d\n", icmplen, Sock_ntop_host(pr->sarecv, pr->salen), icmp->icmp_type, icmp->icmp_code);
    }
}

void init_v6(){
#ifdef IPV6
    int on = 1;
    if (verbose == 0){
        /// install a filter that only passer ICMP6-ECHO_REPLY unless verbose
        struct icmp6_filter myfilt;
        ICMP6_FILTER_SETBLOCKALL(&myfilt);
        ICMP6_FILTER_SETPASS(ICMP6_ECHO_REPLY, &myfilt);
        setsockopt(sockfd, IPPROTO_IPV6, ICMP6_FILTER, &myfilt, sizeof(myfilt));
        ///ignore error return; the filter is an optimaization
    }
    ///ignore errer returned below; we just won't receive the hop limit
#ifdef IPV6IPV6_RECVHOPLIMIT
    setsockopt(sockfd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof(on));
#else
    setsockopt(sockfd, IPPROTO_IPV6, IPV6_HOPLIMIT, &on, sizeof on);
#endif
#endif
}

void proc_v6(char* ptr, ssize_t len, struct  msghdr* msg, struct timeval *tvrecv){
#ifdef IPV6
    double rtt;
    struct icmp6_hdr *icmp6;
    struct timeval *tvsend;
    struct cmsghdr* cmsg;
    int hlim;

    icmp6 = (struct icmp6_hdr*)ptr; /// start of IP header
    if(len < 8){/// malformed packet
        return;
    }

    if (icmp6->icmp6_type == ICMP6_ECHOREPLY){
        if (icmp6->icmp6_id != pid){
            return; /// not a response to our ECHO_REQUEST
        }
        if (len < 16){
            return; /// not enough data to use
        }
        tvsend = (struct timeval*)(icmp6+1);
        tv_sub(tvrecv, tvsend);
        rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec/1000.0;
        hlim = -1;
        for(cmsg = CMSG_FIRSTHDR(msg); cmsg != nullptr; cmsg = CMSG_NXTHDR(msg, cmsg)){
            if(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_HOPLIMIT){
                hlim = *(u_int32_t*)CMSG_DATA(cmsg);
                break;
            }
        }
        printf("%d bytes from %s: seq=%u, hlim=", len, Sock_ntop_host(pr->sarecv, pr->salen),
               icmp6->icmp6_seq);
        if(hlim == -1){
            printf("???");  ///ancillary data missing
        }
        else{
            printf("%d", hlim);d
        }
        printf(", rtt=%.3f ms\n", rtt);
    }
    else if (verbose){
        printf("%d bytes from %s : type=%d, code=%d\n", len, Sock_ntop_host(pr->sarecv, pr->salen), icmp6->icmp6_type, icmp6->icmp6_code);
    }
#endif
}

void send_v4(){
    int len;
    struct icmp* icmp;
    icmp = (struct icmp*)sendbuf;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_id = pid;
    icmp->icmp_seq = nsent++;
    memset(icmp->icmp_data, 0xa5, datalen); ///fill with patter
    Gettimeofday((struct timeval*)icmp->icmp_data, nullptr);
    len = 8+datalen; /// checksum ICMP header and data
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = in_cksum((u_short*)icmp, len);
    Sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
}

void send_v6()
{
#ifdef	IPV6
    int					len;
	struct icmp6_hdr	*icmp6;

	icmp6 = (struct icmp6_hdr *) sendbuf;
	icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
	icmp6->icmp6_code = 0;
	icmp6->icmp6_id = pid;
	icmp6->icmp6_seq = nsent++;
	memset((icmp6 + 1), 0xa5, datalen);	/* fill with pattern */
	Gettimeofday((struct timeval *) (icmp6 + 1), NULL);

	len = 8 + datalen;		/* 8-byte ICMPv6 header */

	Sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
		/* 4kernel calculates and stores checksum for us */
#endif	/* IPV6 */
}

void readloop(){
    int size;
    char recvbuf[BUFFSIZE], controlbuf[BUFFSIZE];
    struct msghdr msg;
    struct iovec iov;
    ssize_t n;
    struct timeval tval;

    sockfd = Socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
    setuid(getuid());   ///don't need special permissions any more
    if (pr->finit){
        pr->finit();
    }
    size = 60 * 1024; ///ok if setsockopt fails
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    sig_alrm(SIGALRM); ///SEND FIRST PACKET

    iov.iov_base = recvbuf;
    iov.iov_len = sizeof recvbuf;
    msg.msg_name = pr->sarecv;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = controlbuf;
    for(;;){
        msg.msg_namelen = pr->salen;
        msg.msg_controllen = sizeof controlbuf;
        n = recvmsg(sockfd, &msg, 0);
        if (n < 0){
            if (errno == EINTR){
                continue;
            }
            else{
                err_sys("recvmsg error");
            }
        }
        Gettimeofday(&tval, nullptr);
        pr->fproc(recvbuf, n, &msg, &tval);
    }
}

int main(int argc, char** argv) {
    /// The length of ip datagram is 84. 20 bytes ipv4 header + 8 bytes icmp header + 56 bytes 可选数据量
    datalen = 56;   ///data that goes with ICMP echo request
    struct addrinfo *ai;
    char *h;
    opterr = 0; /// don't want getopt() writing to stderr
    int c;
    while ((c=getopt(argc, argv, "v")) != -1){
        switch (c) {
            case 'v':{
                ++verbose;
                break;
            }
            case '?':{
                err_quit("unrecognized option:%c", c);
            }
        }
    }
    if (optind != argc-1){
        err_quit("usage: ping [-v] <hostname>");
    }
    host = argv[optind];
    pid = getpid() & 0xffff; ///icmp id field is 16 bits
    Signal(SIGALRM, sig_alrm);
    ai= Host_serv(host, nullptr, 0, 0);
    h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
    printf("PING %s (%s) : %d data bytes\n", ai->ai_canonname ? ai->ai_canonname : h, h, datalen);
    ///initialize according to protocol
    if (ai->ai_family == AF_INET){
        pr = &proto_v4;
#ifdef IPV6
    } else if (ai->ai_family == AF_INET6){
        pr = &proto_v6;
        if(IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6 *) ai->ai_addr)->sin6_addr))){
            err_quit("connot ping IPv4-mapped IPv6 address");
        }
#endif
    } else{
        err_quit("unknown address family %d", ai->ai_family);
    }
    pr->sasend = ai->ai_addr;
    pr->sarecv = static_cast<sockaddr *>(Calloc(1, ai->ai_addrlen));
    pr->salen = ai->ai_addrlen;
    readloop();
    return 0;
}
