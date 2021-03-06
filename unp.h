/* include unph */
/* Our own header.  Tabs are set for 4 spaces, not 8 */

#ifndef	__unp_h
#define	__unp_h

#include <sys/socket.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
/* The structure returned by recvfrom_flags() */
struct unp_in_pktinfo {
    struct in_addr	ipi_addr;	/* dst IPv4 address */
    int				ipi_ifindex;/* received interface index */
};

#define	MAXLINE		4096	/* max text line length */
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
constexpr auto LISTENQ = 1024	/* 2nd argument to listen() */;
/* default file access permissions for new files */
#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
/* default permissions for new directories */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define	UNIXSTR_PATH	"/tmp/unix.str"	/* Unix domain stream */
#define	UNIXDG_PATH		"/tmp/unix.dg"	/* Unix domain datagram */

void str_cli(FILE *fp, int sockfd);
void str_cli_select(FILE *fp, int sockfd);
void str_cli_select_shutdown(FILE *fp, int sockfd);
void str_echo(int sockfd);
void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen);
void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen);

void Write(int fd, void *ptr, size_t nbytes);
void Writen(int, void *, size_t);
size_t Read(int fd, void *ptr, size_t nbytes);
size_t Readline(int fd, void *ptr, size_t maxlen);
int Open(const char *pathname, int oflag, mode_t mode);
void Close(int fd);
void Dup2(int, int);
void *Malloc(size_t size);
pid_t Fork(void);
pid_t Waitpid(pid_t pid, int *iptr, int options);
int Fcntl(int fd, int cmd, int arg);
int Fcntl(int fd, int cmd, int arg);
void* Calloc(size_t n, size_t size);
void* Mmap(void *, size_t, int, int, int, off_t);
int Ioctl(int fd, int request, void *arg);
void Sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen);
void Sigemptyset(sigset_t *);
void Sigaddset(sigset_t *set, int signo);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oset);
void Pipe(int *fds);
void Gettimeofday(struct timeval *tv, void *foo);

ssize_t Read_fd(int fd, void *ptr, size_t nbytes, int *recvfd);
ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd);
ssize_t Write_fd(int fd, void *ptr, size_t nbytes, int sendfd);
//error
void err_quit(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_ret(const char *fmt, ...);
void err_msg(const char *fmt, ...);

void Fclose(FILE *fp);
FILE* Fdopen(int fd, const char *type);
char * Fgets(char *ptr, int n, FILE *stream);
FILE * Fopen(const char *filename, const char *mode);
void Fputs(const char *ptr, FILE *stream);

//signal
typedef	void	Sigfunc(int);	/* for signal handlers */
Sigfunc * Signal(int signo, Sigfunc *func);

//socket
int Socket(int family, int type, int protocol);
void Listen(int fd, int backlog);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Shutdown(int fd, int how);
#define HAVE_POLL
#ifdef	HAVE_POLL
int Poll(struct pollfd *fdarray, unsigned long nfds, int timeout);
#endif
void Sendto(int fd, const void *ptr, size_t nbytes, int flags,
       const struct sockaddr *sa, socklen_t salen);
void	 Send(int, const void *, size_t, int);
ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags,
         struct sockaddr *sa, socklen_t *salenptr);
ssize_t	 Recv(int, void *, size_t, int);
void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
void Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr);
int Readable_timeo(int fd, int sec);
void Socketpair(int family, int type, int protocol, int *fd);
int		 Sockatmark(int);

void Inet_pton(int family, const char *strptr, void *addrptr);
const char * Inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
char *Sock_ntop(const struct sockaddr *sa, socklen_t salen);

int Tcp_connect(const char *host, const char *serv);
int Tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);
int Udp_client(const char *host, const char *serv, sockaddr **saptr, socklen_t *lenptr);
int Udp_server(const char *host, const char *serv, socklen_t *addrlenp);

char * Sock_ntop_host(const struct sockaddr *sa, socklen_t salen);

struct addrinfo * Host_serv(const char *host, const char *serv, int family, int socktype);

void Mcast_join(int sockfd, const struct sockaddr *grp, socklen_t grplen,
           const char *ifname, u_int ifindex);

int family_to_level(int family);

void Mcast_set_loop(int sockfd, int onoff);

int sockfd_to_family(int sockfd);

void sock_set_wild(struct sockaddr *sa, socklen_t salen);

void sock_set_addr(struct sockaddr *sa, socklen_t salen, const void *addr);

char *If_indextoname(unsigned int index, char *name);

ssize_t recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp, struct sockaddr *sa,
                       socklen_t *salenptr, struct unp_in_pktinfo *pktp);
ssize_t Recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
               struct sockaddr *sa, socklen_t *salenptr, struct unp_in_pktinfo *pktp);
void sock_set_port(struct sockaddr *sa, socklen_t salen, int port);
int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen);
int Sock_bind_wild(int sockfd, int family);
int sock_bind_wild(int sockfd, int family);
int sock_get_port(const struct sockaddr *sa, socklen_t salen);

ssize_t Readn(int fd, void *ptr, size_t nbytes);
#endif	/* __unp_h */
