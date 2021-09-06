#include <cerrno>
#include <cstdlib>
#include <algorithm>
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
    char	buf[MAXLINE];
    int n;

    FD_ZERO(&rset);
    heartbeat_cli(sockfd, 1, 5);
    for (;;){
        if(stdineof == 0){
            FD_SET(fileno(fp), &rset);
        }

        FD_SET(sockfd, &rset);

        maxfdp1 = std::max(fileno(fp), sockfd) + 1;
        int nResult = Select(maxfdp1, &rset, nullptr, nullptr, nullptr);
        if(nResult == EINTR){
            continue;
        }
        else{
            err_ret("select error");
        }
        if (FD_ISSET(sockfd, &rset)){
            if ((n=Read(sockfd, buf, MAXLINE)) == 0){
                if (stdineof == 1){
                    return; /// normal termination
                }
                else{
                    err_quit("str_cli: server terminated prematurely");
                }
            }
            Writen(fileno(stdout), buf, n);
        }
        if(FD_ISSET(fileno(fp), &rset)){
            if ((n=Read(fileno(fp), buf, MAXLINE)) == 0){
                stdineof = 1;
                Shutdown(sockfd, SHUT_WR);    /// send FIN
                FD_CLR(fileno(fp), &rset);
                continue;
            }

            Writen(sockfd, buf, n);
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
    servaddr.sin_port = htons(1313);
    Inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0);
    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);
    return 0;
}
