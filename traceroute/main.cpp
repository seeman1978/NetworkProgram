#include "trace.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <cerrno>

int gotalarm;

void sig_alrm(int signo){
    gotalarm = 1;   ///set flag to note that alarm occurred
    return;
}

void tv_sub(struct timeval *out, struct timeval *in){
    if ((out->tv_usec -= in->tv_usec) < 0){
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

const char* icmpcode_v6(int code){
#ifdef IPV6
    static char errbuf[100];
    switch(code){
        case ICMP6_DST_UNREACH_NOROUTE:
            return "no route to host";
        case ICMP6_DST_UNREACH_ADMIN:
            return "adminsitratively prohibited";
        case ICMP6_DST_UNREACH_NOTNEIGHBOR:
            return "not a neighbor";
        case ICMP6_DST_UNREACH_ADDR:
            return "address unreachable";
        case ICMP6_DST_UNREACH_NOPORT:
            return "port unreachable";
        default:
            sprintf(errbuf, "[unknown code %d]", code);
            return errbuf;
    }
#endif
}

int recv_v6(int seq, struct timeval *tv){
#ifdef IPV6
    int hlen2, ret, icmp6len;
    socklen_t  len;
    ssize_t n;

    struct ip6_hdr *hip6;
    struct icmp6_hdr *icmp6;
    struct udphdr *udp;

    gotalarm = 0;
    alarm(3);

    for (;;) {
        if (gotalarm){
            return -3;
        }
        len = pr->salen;
        n = recvfrom(recvfd, recvbuf, sizeof recvbuf, 0, pr->sarecv, &len);
        if (n < 0){
            if (errno == EINTR){
                continue;
            }
            else{
                err_sys("recvfrom error");
            }
        }

        icmp6 = (struct icmp6_hdr*)recvbuf;
        if ((icmp6len = n) < 8){
            continue;
        }
        if (icmp6->icmp6_type == ICMP6_TIME_EXCEEDED && icmp6->icmp6_code == ICMP_TIME_EXCEED_TRANSIT) {
            if (icmp6len < 8 + sizeof(struct ip6_hdr) + 4) {
                continue;
            }
            hip6 = (struct ip6_hdr *) (recvbuf + 8);
            hlen2 = sizeof(struct ip6_hdr);
            udp = (struct udphdr *) (recvbuf + 8 + hlen2);
            if (hip6->ip6_nxt == IPPROTO_UDP && udp->uh_sport == htons(sport) && udp->uh_dport == htons(dport + seq)) {
                ret = -2;
                break;
            } else if (icmp6->icmp6_type == ICMP6_DST_UNREACH) {
                if (icmp6len < 8 + sizeof(struct ip6_hdr) + 4) {
                    continue;
                }
                hip6 = (struct ip6_hdr *) (recvbuf + 8);
                hlen2 = sizeof(struct ip6_hdr);
                udp = (struct udphdr *) (recvbuf + 8 + hlen2);
                if (hip6->ip6_nxt == IPPROTO_UDP && udp->uh_sport == htons(sport) &&
                    udp->uh_dport == htons(dport + seq)) {
                    if (icmp6->icmp6_code == ICMP6_DST_UNREACH_NOPORT) {
                        return -1;  /// have reached destination
                    } else {
                        ret = icmp6->icmp6_code;
                    }
                    break;
                }
            } else if (verbose) {
                printf(" (from %s:type=%d, code= %d)\n", Sock_ntop_host(pr->sarecv, pr->salen), icmp6->icmp6_type,
                       icmp6 > icmp6_code);
            }
        }
    }
    alarm(0);
    Gettimeofday(tv, nullptr);
    return ret;
#endif
}

int recv_v4(int seq, struct timeval *tv){
    int hlen1, hlen2, ret, icmplen;
    socklen_t  len;
    ssize_t n;

    struct ip *ip, *hip;
    struct icmp *icmp;
    struct udphdr *udp;

    gotalarm = 0;
    alarm(3);

    for (;;) {
        if (gotalarm){
            return -3;
        }
        len = pr->salen;
        n = recvfrom(recvfd, recvbuf, sizeof recvbuf, 0, pr->sarecv, &len);
        if (n < 0){
            if (errno == EINTR){
                continue;
            }
            else{
                err_sys("recvfrom error");
            }
        }
        ip = (struct ip*)recvbuf;
        hlen1 = ip->ip_hl << 2;

        icmp = (struct icmp*)(recvbuf + hlen1);
        if ((icmplen = n-hlen1) < 8){
            continue;
        }
        if (icmp->icmp_type == ICMP_TIMXCEED && icmp->icmp_code == ICMP_TIMXCEED_INTRANS){
            if (icmplen < 8+sizeof(struct ip)){
                continue;
            }
            hip = (struct ip*)(recvbuf+hlen1+8);
            hlen2 = hip->ip_hl << 2;
            if (icmplen < 8+hlen2+4){
                continue;
            }
            udp = (struct udphdr*)(recvbuf+hlen1+8+hlen2);
            if (hip->ip_p == IPPROTO_UDP && udp->uh_sport == htons(sport) &&
                udp->uh_dport == htons(dport+seq)){
                ret = -2;
                break;
            }
        }
        else if (icmp->icmp_type == ICMP_UNREACH){
            if (icmplen < 8 + hlen2 + 4){
                continue;
            }
            udp = (struct udphdr*)(recvbuf + hlen1 + 8 + hlen2);
            if (hip->ip_p == IPPROTO_UDP && udp->uh_sport == htons(sport) &&
                udp->uh_dport == htons(dport+seq)){
                if (icmp->icmp_code == ICMP_UNREACH_PORT){
                    ret = -1;///have reached destination
                }
                else{
                    ret = icmp->icmp_code;
                }
                break;
            }
        }
        if (verbose){
            printf(" (from %s:type=%d, code= %d)\n", Sock_ntop_host(pr->sarecv, pr->salen), icmp->icmp_type, icmp->icmp_code);
        }
    }
    alarm(0);
    Gettimeofday(tv, nullptr);
    return ret;
}

const char *
icmpcode_v4(int code)
{
    static char errbuf[100];
    switch (code) {
        case  0:	return("network unreachable");
        case  1:	return("host unreachable");
        case  2:	return("protocol unreachable");
        case  3:	return("port unreachable");
        case  4:	return("fragmentation required but DF bit set");
        case  5:	return("source route failed");
        case  6:	return("destination network unknown");
        case  7:	return("destination host unknown");
        case  8:	return("source host isolated (obsolete)");
        case  9:	return("destination network administratively prohibited");
        case 10:	return("destination host administratively prohibited");
        case 11:	return("network unreachable for TOS");
        case 12:	return("host unreachable for TOS");
        case 13:	return("communication administratively prohibited by filtering");
        case 14:	return("host recedence violation");
        case 15:	return("precedence cutoff in effect");
        default:	sprintf(errbuf, "[unknown code %d]", code);
            return errbuf;
    }
}

struct proto proto_v4
{
	icmpcode_v4, recv_v4, nullptr, nullptr, nullptr, nullptr, 0, IPPROTO_ICMP,
		IPPROTO_IP, IP_TTL
};

#ifdef IPV6
struct proto proto_v6 {icmpcode_v6, recv_v6, nullptr, nullptr, nullptr, nullptr, 0,
IPPROTO_ICMPV6, IPPROTO_IPV6, IPV6_UNICAST_HOPS};
#endif

void traceloop(){
    int seq, code, done;
    double rtt;
    struct rec *rec;
    struct timeval tvrecv;
    recvfd = Socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
    setuid(getuid());
#ifdef IPV6
    if (pr->sasend->sa_family == AF_INET6 && verbose == 0){
        struct icmp6_filter myfilt;
        ICMP6_FILTER_SETBLOCKALL(&myfilt);
        ICMP6_FILTER_SETPASS(ICMP6_TIME_EXCEEDED, &myfilt);
        ICMP6_FILTER_SETPASS(ICMP6_DST_UNREACH, &myfilt);
        setsockopt(recvfd, IPPROTO_IPV6, ICMP6_FILTER, &myfilt, sizeof(myfilt));
    }
#endif
    /// give udp socket bind source port
    sendfd = Socket(pr->sasend->sa_family, SOCK_DGRAM, 0);
    pr->sabind->sa_family = pr->sasend->sa_family;
    sport = (getpid() & 0xffff)|0x8000; ///our source udp port
    sock_set_port(pr->sabind, pr->salen, htons(sport));
    Bind(sendfd, pr->sabind, pr->salen);
    sig_alrm(SIGALRM);

    seq = 0;
    done = 0;
    for (ttl = 1; ttl <= max_ttl && done == 0; ttl++) {
        Setsockopt(sendfd, pr->ttllevel, pr->ttloptname, &ttl, sizeof(int));
        bzero(pr->salast, pr->salen);
        printf("%2d ", ttl);
        fflush(stdout);

        for(probe = 0; probe < nprobes; probe++){
            rec = (struct rec*)sendbuf;
            rec->rec_seq = ++seq;
            rec->rec_ttl = ttl;
            Gettimeofday(&rec->rec_tv, nullptr);
            sock_set_port(pr->sasend, pr->salen, htons(dport + seq));
            Sendto(sendfd, sendbuf, datalen, 0, pr->sasend, pr->salen);
            if((code = (*pr->recv)(seq, &tvrecv)) == -3){
                printf(" *");/// timeout , no reply
            }
            else{
                char str[NI_MAXHOST];
                if (sock_cmp_addr(pr->sarecv, pr->salast, pr->salen) != 0){
                    if (getnameinfo(pr->sarecv, pr->salen, str, sizeof(str), nullptr, 0, 0) == 0){
                        printf(" %s (%s) ", str, Sock_ntop_host(pr->sarecv, pr->salen));
                    }
                    else{
                        printf(" %s", Sock_ntop_host(pr->sarecv, pr->salen));
                    }
                    memcpy(pr->salast, pr->sarecv, pr->salen);
                }
                tv_sub(&tvrecv, &rec->rec_tv);
                rtt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec/1000.0;
                printf("    %3.f ms", rtt);
                if (code == -1){
                    done++;
                }
                else if(code >= 0){
                    printf(" (ICMP %s)", (*pr->icmpcode)(code));
                }
            }
            fflush(stdout);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    datalen = sizeof(struct rec);
    max_ttl = 30;
    nprobes = 3;
    dport = 32768 + 666;

	int c;
	struct addrinfo* ai;
	char* h;
	opterr = 0;
	while ((c=getopt(argc, argv, "m:v")) != -1)
	{
		switch (c) {
			case 'm': {
				if ((max_ttl = atoi(optarg)) <= 1)
				{
					err_quit("invalid -m value");
					break;
				}
			}
			case 'v': {
				verbose++;
				break;
			}
			case '?': {
				err_quit("unrecognized option : %c", c);
			}
		}
	}
	if (optind != argc - 1) {
		err_quit("usage:traceroute [ -m <maxttl> -v] <hostname>");
	}
	host = argv[optind];
	pid = getpid();

	Signal(SIGALRM, sig_alrm);
	ai = Host_serv(host, nullptr, 0, 0);
	h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
	printf("traceroute to %s(%s):%d hops max, %d data bytes\n",
		ai->ai_canonname ? ai->ai_canonname : h, h, max_ttl, datalen);
	/// init according to protocol;
	if (ai->ai_family == AF_INET) {
		pr = &proto_v4;
#ifdef IPV6
	}
	else if (ai->ai_family == AF_INET6) {
		pr = &proto_v6;
		if (IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6*)ai->ai_addr)->sin6_addr))) {
			err_quit("can not traceroute IPV4-MAPPED ipV6 ADDRESS");
		}
#endif
	}
	else {
		err_quit("unknown address family %d", ai->ai_family);
	}
	pr->sasend = ai->ai_addr;
	pr->sarecv = static_cast<sockaddr *>(Calloc(1, ai->ai_addrlen));
	pr->salast = static_cast<sockaddr *>(Calloc(1, ai->ai_addrlen));
	pr->sabind = static_cast<sockaddr *>(Calloc(1, ai->ai_addrlen));
	pr->salen = ai->ai_addrlen;
	traceloop();
	exit(0);
}