#include <cerrno>
#include <cstdlib>
#include <algorithm>
#include <unistd.h>
#include <csignal>
#include <sys/fcntl.h>
#include <strings.h>
#include <cstring>
#include "../../unp.h"

static int servfd;
static int nsec;        /// seconds between each alarm
static int maxnprobes;  /// probes w/no response before quit
static int nprobes;     /// probes since last server response

static void sig_urg(int signo){
    int n;
    char c;
    if((n=recv(servfd, &c, 1, MSG_OOB)) < 0){
        if(errno != EWOULDBLOCK){
            err_sys("recv error");
        }
    }
    nprobes = 0;    /// reset counter
    return; /// may interrupt client code
}

static void sig_alrm(int signo){
    if(++nprobes > maxnprobes){
        fprintf(stderr, "server is unreachable\n");
        exit(0);
    }
    Send(servfd, "1", 1, MSG_OOB);
    alarm(nsec);
    return;
}

void heartbeat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg){
    servfd = servfd_arg;    /// set globals for signal handlers
    if((nsec = nsec_arg) < 1){
        nsec = 1;
    }
    if((maxnprobes = maxnprobes_arg) < nsec){
        maxnprobes = nsec;
    }
    nprobes = 0;
    Signal(SIGURG, sig_urg);
    Fcntl(servfd, F_SETOWN, getpid());

    Signal(SIGALRM, sig_alrm);
    alarm(nsec);
}

//select implement
void str_cli(FILE *fp, int sockfd)
{
    int maxfdp1{0}, stdineof{0};
    fd_set rset;    //read fd sets
    char		sendline[MAXLINE], recvline[MAXLINE];

    heartbeat_cli(sockfd, 1, 5);

    FD_ZERO(&rset);
    for (;;){
        if(stdineof == 0){
            FD_SET(fileno(fp), &rset);
        }

        FD_SET(sockfd, &rset);

        maxfdp1 = std::max(fileno(fp), sockfd) + 1;
        int nResult = select(maxfdp1, &rset, nullptr, nullptr, nullptr);
        if(nResult == -1){
            if(errno == EINTR){
                continue;
            }
            else{
                err_ret("select error");
            }
        }

        if (FD_ISSET(sockfd, &rset)){
            if (Readline(sockfd, recvline, MAXLINE) == 0){
                if (stdineof == 1){
                    return; /// normal termination
                }
                else{
                    err_quit("str_cli: server terminated prematurely");
                }
            }
            Writen(fileno(stdout), recvline, strlen(recvline));
        }

        if(FD_ISSET(fileno(fp), &rset)){
            if (Fgets(sendline, MAXLINE, fp) == nullptr){
                stdineof = 1;
                alarm(0);			/* turn off heartbeat */
                Shutdown(sockfd, SHUT_WR);    /// send FIN
                FD_CLR(fileno(fp), &rset);
                continue;
            }

            Writen(sockfd, sendline, strlen(sendline));
        }
    }
}

int main() {
    int sockfd{0};

    struct sockaddr_in servaddr{};
    //ip v4 tcp
    sockfd=Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1234);
    Inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);
    return 0;
}
