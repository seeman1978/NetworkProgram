#include "trace.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

struct proto proto_v4
{
	icmpcode_v4, recv_v4, nullptr, nullptr, nullptr, nullptr, 0, IPPROTO_ICMP,
		IPPROTO_IP, IP_TTL
};

#ifdef IPV6
struct proto proto_v6 {icmpcode_v6, recv_v6, nullptr, nullptr, nullptr, nullptr, 0,
IPPROTO_ICMPV6, IPPROTO_IPV6, IPV6_UNICAST_HOPS};
#endif

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