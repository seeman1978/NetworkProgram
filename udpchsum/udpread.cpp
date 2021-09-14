#include "udpcksum.h"

struct udpiphdr * udp_read() {
	int len;
	char* ptr;
	struct ether_header* eptr;
	for (;;) {
		ptr = next_pcap(&len);
		switch (datalink) {
			case DLT_NULL:
				return udp_check(ptr + 4, len - 4);
			case DLT_EN10MB:
				eptr = (struct ether_header*)ptr;
				if(ntohs(eptr->ether_type) != ETHERTYPE_IP) {
					err_quit("ethernet type %x not iP", ntohs(eptr->ether_type));
				}
				return udp_check(ptr + 14, len - 14);
			case DLT_SLIP:
				return udp_check(ptr + 24, len - 24);
			case DLT_PPP:
				return udp_check(ptr + 24, len - 24);
			default:
				err_quit("unsupported datalink (%d)", datalink);
		}
	}
}
