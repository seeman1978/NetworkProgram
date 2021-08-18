#include <cstring>
#include	"../unp.h"
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

char* gf_time();

void str_cli(FILE *fp, int sockfd)
{
    int maxfdp1, val, stdineof;
    ssize_t n, nwritten;
    fd_set rset, wset;
	char	to[MAXLINE], fr[MAXLINE];
	char *toiptr, *tooptr, *friptr, *froptr;
	val = Fcntl(sockfd, F_GETFL, 0);
	Fcntl(sockfd, F_SETFL, val|O_NONBLOCK);

	val = Fcntl(STDIN_FILENO, F_GETFL, 0);
	Fcntl(STDIN_FILENO, F_SETFL, val|O_NONBLOCK);

	val = Fcntl(STDOUT_FILENO, F_GETFL, 0);
	Fcntl(STDOUT_FILENO, F_SETFL, val|O_NONBLOCK);

	toiptr = tooptr = to;
	friptr = froptr = fr;
	stdineof = 0;

	maxfdp1 = std::max(std::max(STDIN_FILENO, STDOUT_FILENO), sockfd) + 1;

    for (; ;) {
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if (stdineof == 0 && toiptr < &to[MAXLINE]){
            FD_SET(STDIN_FILENO, &rset); /// read from stdin
        }
        if (friptr < &fr[MAXLINE]){
            FD_SET(sockfd, &rset);  /// read from socket
        }
        if (tooptr != toiptr){
            FD_SET(sockfd, &wset);  /// data to write to socket
        }
        if (froptr != friptr){
            FD_SET(STDOUT_FILENO, &wset); /// data to write to stdout
        }
        Select(maxfdp1, &rset, &wset, nullptr, nullptr);
        if (FD_ISSET(STDIN_FILENO, &rset)){
            if ((n = read(STDIN_FILENO, toiptr, &to[MAXLINE]-toiptr)) < 0){
                if (errno != EWOULDBLOCK){
                    err_sys("read error on stdin");
                }
            }
            else if (n == 0){
                fprintf(stderr, "%s: EOF on stdin\n", gf_time());
                stdineof = 1;   /// all done with stdin
                if (tooptr == toiptr){
                    Shutdown(sockfd, SHUT_WR); /// send FIN
                }
            }
            else{
                fprintf(stderr, "%s: read %ld bytes from stdin \n", gf_time(), n);
                toiptr += n;
                FD_SET(sockfd, &wset);
            }
        }
        if (FD_ISSET(sockfd, &rset)){
            if ((n= read(sockfd, friptr, &fr[MAXLINE]-friptr)) < 0){
                if (errno != EWOULDBLOCK){
                    err_sys("read error no socket");
                }
            }
            else if (n == 0){
                fprintf(stderr, "%s: EOF on socket\n", gf_time());
                if (stdineof){
                    return; /// normal terminate
                }
                else{
                    err_quit("str_cli: server terminated prematurely");
                }
            }
            else{
                fprintf(stderr, "%s: read %ld bytes from socket \n", gf_time(), n);
                friptr += n;    /// just read
                FD_SET(STDOUT_FILENO, &wset);   /// try and write below
            }
        }
        if (FD_ISSET(STDOUT_FILENO, &wset) && ((n = friptr - froptr) > 0)){
            if ((nwritten = write(STDOUT_FILENO, froptr, n)) < 0){
                if (errno != EWOULDBLOCK){
                    err_sys("write error to stdout");
                }
            }
            else{
                fprintf(stderr, "%s: wrote %ld bytes to stdout\n", gf_time(), nwritten);
                froptr += nwritten; /// just written
                if (froptr == friptr){
                    froptr = friptr = fr;   /// back to beginning of buffer
                }
            }
        }
        if (FD_ISSET(sockfd, &wset) && ((n = toiptr - tooptr) > 0)){
            if ((nwritten = write(sockfd, tooptr, n)) < 0){
                if (errno != EWOULDBLOCK){
                    err_sys("write error to socket");
                }
            }
            else{
                fprintf(stderr, "%s: wrote %ld bytes to socket \n", gf_time(), nwritten);
                tooptr += nwritten; /// just written
                if (tooptr == toiptr){
                    toiptr = tooptr = to;   /// back to beginning of buffer
                    if (stdineof){
                        Shutdown(sockfd, SHUT_WR); /// send FIN
                    }
                }
            }
        }
    }
}

//select implement
void str_cli_select(FILE *fp, int sockfd)
{
    int maxfdp1{0};
    fd_set rset;    //read fd sets

    char	sendline[MAXLINE], recvline[MAXLINE];
    FD_ZERO(&rset);
    for (;;){
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = std::max(fileno(fp), sockfd) + 1;
        Select(maxfdp1, &rset, nullptr, nullptr, nullptr);
        if (FD_ISSET(sockfd, &rset)){
            if (Readline(sockfd, recvline, MAXLINE) == 0){
                err_quit("str_cli_select: server terminated prematurely");
            }
            Fputs(recvline, stdout);
        }
        if(FD_ISSET(fileno(fp), &rset)){
            if (Fgets(sendline, MAXLINE, fp) == nullptr){
                return;
            }

            Writen(sockfd, sendline, strnlen(sendline, sizeof sendline));
        }
    }
}

//select and shutdown implement
void str_cli_select_shutdown(FILE *fp, int sockfd)
{
    int maxfdp1{0}, stdineof{0};
    fd_set rset;    //read fd sets
    char buf[MAXLINE];
    size_t n{0};
    FD_ZERO(&rset);

    for (;;){
        if (stdineof == 0){
            FD_SET(fileno(fp), &rset);
        }
        FD_SET(sockfd, &rset);
        maxfdp1 = std::max(fileno(fp), sockfd) + 1;
        Select(maxfdp1, &rset, nullptr, nullptr, nullptr);
        if (FD_ISSET(sockfd, &rset)){
            if ((n=Read(sockfd, buf, MAXLINE)) == 0){
                if (stdineof == 1){
                    return;
                }
                else{
                    err_quit("str_cli: server terminated prematurely");
                }
            }
            Write(fileno(stdout), buf, n);
        }
        if(FD_ISSET(fileno(fp), &rset)){
            if ((n=Read(fileno(fp), buf, MAXLINE)) == 0){
                stdineof = 1;
                Shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            Writen(sockfd, buf, n);
        }
    }
}
