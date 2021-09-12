#include "unpicmpd.h"
#include <sys/un.h>
struct client
{
	int connfd;
	int family;
	int lport;
} client[FD_SETSIZE];

int fd4, fd6, listenfd, maxi, maxfd, nready;
fd_set rset, allset;
struct sockaddr_un cliaddr;
int readable_conn(int);
int readable_listen(void);
int readable_v4();
int readable_v6();