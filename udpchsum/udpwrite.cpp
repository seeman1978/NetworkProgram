#include <strings.h>

#include "udpcksum.h"
int rawfd;
void open_output() {
	int on = 1;
	rawfd = Socket(dest->sa_family, SOCK_RAW, 0);
	Setsockopt(rawfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof on);
}

void udp_write(char* buf, int userlen) {
	struct udpiphdr* ui;
	struct ip* ip;
	ip = (struct ip*)buf;
	ui = (struct udpiphdr*)buf;
	bzero(ui, sizeof(*ui));
	ui->ui_len = htons((uint16_t)(sizeof(struct udphdr) + userlen));
	userlen += sizeof(struct udpiphdr);

	ui->ui_pr = IPPROTO_UDP;
	ui->ui_src.s_addr = ((struct sockaddr_in*)local)->sin_addr.s_addr;
	ui->ui_dst.s_addr = ((struct sockaddr_in*)dest)->sin_addr.s_addr;
	ui->ui_sport = ((struct sockaddr_in*)local)->sin_port;
	ui->ui_dport = ((struct sockaddr_in*)dest)->sin_port;
	ui->ui_ulen = ui->ui_len;
	if (zerosum == 0) {
#if 1
		if ((ui->ui_sum = in_cksum((u_int16_t*)ui, userlen)) == 0) {
			ui->ui_sum = 0xffff;
		}
#else
		ui->ui_sum = ui->ui_len;
#endif		
	}

	ip->ip_v = IPVERSION;
	ip->ip_hl = sizeof(struct ip) >> 2;
	ip->ip_tos = 0;
#if defined(linux) || defined(__OpenBSD__)
	ip->ip_len = htons(userlen);
#else
	ip->ip_len = userlen;
#endif
	ip->ip_id = 0;
	ip->ip_off = 0;
	ip->ip_ttl = TTL_OUT;

	Sendto(rawfd, buf, userlen, 0, dest, destlen);
}