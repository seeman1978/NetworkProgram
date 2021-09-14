#include <cstdio>
#include <setjmp.h>
#include <unistd.h>

#include "udpcksum.h"

void test_udp() {
	volatile int nsent{ 0 }, timeout{ 3 };
	struct udpiphdr& ui;
	Signal(SIGALRM, sig_alrm);

	if (sigsetjmp(jmpbuf, 1)) {
		if (nsent >= 3) {
			err_quit("no response");
		}
		printf("timeout\n");
		timeout *= 2;
	}
	canjump = 1;
	send_dns_query();
	nsent++;
	alarm(timeout);
	ui = udp_read();
	canjump = 0;
	alarm(0);
	if(ui->ui_sum == 0) {
		printf("udp checksums off\n");
	}
	else {
		printf("udp checksum on\n");
	}
	if (verbose) {
		printf("received udp checksum=%s\n", ntohs(ui->ui_sum));
	}
}
