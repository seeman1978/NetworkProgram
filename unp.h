/* include unph */
/* Our own header.  Tabs are set for 4 spaces, not 8 */

#ifndef	__unp_h
#define	__unp_h

#include <sys/socket.h>
#include <stdio.h>

#define	MAXLINE		4096	/* max text line length */
void str_cli(FILE *fp, int sockfd);
void str_echo(int sockfd);

void Writen(int, void *, size_t);
size_t Readline(int fd, void *ptr, size_t maxlen);

void Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void err_quit(const char *fmt, ...);
void err_sys(const char *fmt, ...);

void Fclose(FILE *fp);

FILE* Fdopen(int fd, const char *type);

char * Fgets(char *ptr, int n, FILE *stream);

FILE * Fopen(const char *filename, const char *mode);

void Fputs(const char *ptr, FILE *stream);

#endif	/* __unp_h */
