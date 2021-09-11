#include "trace.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>

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
            sock_set_port(pr-.sasend, pr->salen, htons(dport + seq));
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