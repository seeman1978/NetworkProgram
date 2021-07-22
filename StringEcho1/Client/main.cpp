#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdarg>

#define MAXLINE 1024
int		daemon_proc;		/* set nonzero by daemon_init() */

static void
err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
    int		errno_save, n;
    char	buf[MAXLINE + 1];

    errno_save = errno;		/* value caller might want printed */
#ifdef	HAVE_VSNPRINTF
    vsnprintf(buf, MAXLINE, fmt, ap);	/* safe */
#else
    vsprintf(buf, fmt, ap);					/* not safe */
#endif
    n = strlen(buf);
    if (errnoflag)
        snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
    strcat(buf, "\n");

    if (daemon_proc) {
        syslog(level, buf);
    } else {
        fflush(stdout);		/* in case stdout and stderr are the same */
        fputs(buf, stderr);
        fflush(stderr);
    }
    return;
}

void
err_quit(const char *fmt, ...)
{
    va_list		ap;

    va_start(ap, fmt);
    err_doit(0, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(1);
}

void str_cli(FILE *fp, int sockfd){
    char sendline[MAXLINE], recvline[MAXLINE];
    while (fgets(sendline, MAXLINE, fp) != nullptr) {
        write(sockfd, sendline, strlen(sendline));
        if (read(sockfd, recvline, MAXLINE) == 0) {
            err_quit("str_cli: server terminated prematurely");
        }
        fputs(recvline, stdout);
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    int sockfd{0}, n{0};
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr{};
    //ip v4 tcp
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "socket error " << sockfd << '\n';
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1313);
    if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0){
        std::cout << "inet_pton error for " << "127.0.0.1" << '\n';
    }
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        //if server does not bind or listen, connect will fail.
        std::cout << "connect error" << '\n';
        return 0;
    }

    str_cli(stdin, sockfd);

    return 0;
}

