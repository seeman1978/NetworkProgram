#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <syslog.h>		/* for syslog() */
#include <cstdarg>
#define MAXLINE 1024

int		daemon_proc;		/* set nonzero by daemon_init() */

void
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
err_sys(const char *fmt, ...)
{
    va_list		ap;

    va_start(ap, fmt);
    err_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(1);
}

void str_echo(int sockfd){
    ssize_t n=0;
    char buf[1024];

    do {
        while((n=read(sockfd, buf, 1024)) >0){
            write(sockfd, buf, n);
        }
        if(n < 0 && errno  == EINTR){
            continue;
        } else if (n < 0){
            err_sys("str_echo:read error");
            break;
        } else{
            break;
        }
    } while (true);
}

int main() {
    int listenfd, connfd;
    pid_t childpid;
    socklen_t chilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);

    bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 100);
    for (;  ; ) {
        chilen = sizeof(cliaddr);
        connfd  = accept(listenfd, (sockaddr*)&cliaddr, &chilen);
        if ((childpid = fork()) < 0) {
            err_sys("fork failed");
        } else if (childpid == 0) {	/* child */
            close(listenfd);
            str_echo(connfd);
            exit(0);
        } else {		/* parent */
            close(connfd);
        }
    }
    return 0;
}
