#include <iostream>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include "../../unp.h"

int main() {
    int mib[4], val;
    size_t len;
    mib[0] = CTL_NET;
    mib[1] = AF_INET;
    mib[2] = IPPROTO_UDP;
    mib[3] = UDPCTL_CHECKSUM;
    len = sizeof val;
    Sysctl(mib, 4, &val, &len, nullptr, 0);
    printf("udp checksum flag: %d\n", val);
    return 0;
}
