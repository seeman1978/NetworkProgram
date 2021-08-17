#include <unistd.h>
#include <cerrno>
#include "../unp.h"
#include <sys/socket.h>

ssize_t read_cred(int, void*, size_t, struct ucred*);

void
str_echo(int sockfd)
{
	ssize_t		n;
	int i;
	char		buf[MAXLINE];
    struct ucred cred;
again:
	while ( (n = read_cred(sockfd, buf, MAXLINE, &cred)) > 0){
	    printf("PID of sender = %d\n", cred.pid);
	    printf("real user ID = %d\n", cred.uid);
	    printf("real group ID = %d\n", cred.gid);
	    Writen(sockfd, buf, n);
	}

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
}
