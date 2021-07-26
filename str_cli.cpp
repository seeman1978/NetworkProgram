#include <cstring>
#include	"unp.h"
#include <algorithm>
void
str_cli(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != nullptr) {

		Writen(sockfd, sendline, strlen(sendline));

		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");

		Fputs(recvline, stdout);
	}
}
//select mplement
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
