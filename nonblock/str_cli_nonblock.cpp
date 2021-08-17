#include <cstring>
#include	"../unp.h"
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

void
str_cli(FILE *fp, int sockfd)
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



	while (Fgets(sendline, MAXLINE, fp) != nullptr) {

		Writen(sockfd, sendline, strlen(sendline));

		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");

		Fputs(recvline, stdout);
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
    int n{0};
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
