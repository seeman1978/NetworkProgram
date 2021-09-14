#include<cstring>

#include "../unp.h"
void send_dns_query() {
	size_t nbytes;
	char* buf, * ptr;

	buf = Malloc(sizeof(struct udpiphdr) + 100);
	ptr = buf + sizeof(struct updiphdr);
	*((uint16_t*)ptr) = htons(1234);
	ptr += 2;
	*((uint16_t*)ptr) = htons(0x0100);
	ptr += 2;
	*((uint16_t*)ptr) = htons(1);
	ptr += 2;
	*((uint16_t*)ptr) =0;
	ptr += 2;
	*((uint16_t*)ptr) = 0;
	ptr += 2;
	*((uint16_t*)ptr) = 0;
	ptr += 2;

	memcpy(ptr, "\001a\014root-servers\003net\000", 20);
	ptr += 20;
	*((uint16_t*)ptr) = htons(1);
	ptr += 2;
	*((uint16_t*)ptr) = htons(1);
	ptr += 2;
}