/* include unph */
/* Our own header.  Tabs are set for 4 spaces, not 8 */

#ifndef	__unp_h
#define	__unp_h

#include <sys/socket.h>
#include <stdio.h>

#define	MAXLINE		4096	/* max text line length */
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
/* default file access permissions for new files */
#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
/* default permissions for new directories */

void str_cli(FILE *fp, int sockfd);
void str_cli_select(FILE *fp, int sockfd);
void str_cli_select_shutdown(FILE *fp, int sockfd);
void str_echo(int sockfd);

void Write(int fd, void *ptr, size_t nbytes);
void Writen(int, void *, size_t);
size_t Read(int fd, void *ptr, size_t nbytes);
size_t Readline(int fd, void *ptr, size_t maxlen);
int Open(const char *pathname, int oflag, mode_t mode);
void Close(int fd);

//error
void err_quit(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_ret(const char *fmt, ...);

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
#endif	/* __unp_h */
