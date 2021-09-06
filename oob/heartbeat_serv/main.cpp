#include <cerrno>
#include <cstdlib>
#include "../../unp.h"

static int servfd;
static int nsec;        /// seconds between each alarm
static int maxnalarms;  /// alarms w/no client probe before quit
static int nprobes;     /// probes since last client probe

static void sig_urg(int signo){
    int n;
    char c;
    if((n=recv(servfd, &c, 1, MSG_OOB)) < 0){
        if(errno != EWOULDBLOCK){
            err_sys("recv error");
        }
    }
    Send(servfd, &c, 1, MSG_OOB);   /// echo back out-of-band byte
    nprobes = 0;    /// reset counter
    return; /// may interrupt client code
}

static void sig_alrm(int signo){
    if(++nprobes > maxnalarms){
        fprintf(stderr, "no probes from client\n");
        exit(0);
    }

    alarm(nsec);
    return;
}

void heartbeat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg){
    servfd = servfd_arg;
    if((nsec = nsec_arg) < 1){
        nsec = 1;
    }
    if((maxnalarms = maxnalarms_arg) < nsec){
        maxnalarms = nsec;
    }
    Signal(SIGURG, sig_urg);
    Fcntl(servfd, F_SETOWN, getpid());

    Signal(SIGALRM, sig_alrm);
    alarm(nsec);
}

void str_echo(int sockfd)
{
    ssize_t		n;
    char		buf[MAXLINE];
    heartbeat_serv(sockfd, 1, 5)
again:
    while ( (n = read(sockfd, buf, MAXLINE)) > 0){
        Writen(sockfd, buf, n);
    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        err_sys("str_echo: read error");
}

int main() {
    int listenfd, connfd;
    pid_t childpid;
    socklen_t chilen;
    struct sockaddr_in cliaddr{}, servaddr{};

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    Listen(listenfd, 100);
    Signal(SIGCHLD, sig_chld);  //处理僵死进程
    //macos系统中，由于内核将重启被中断的系统调用，于是accept不会返回错误。
    for (;  ; ) {
        chilen = sizeof(cliaddr);
        connfd  = accept(listenfd, (sockaddr*)&cliaddr, &chilen);
        if(connfd < 0){
            if(errno == EINTR){
                continue;
            }
            else{
                err_sys("accept error");    //打印错误消息，并退出程序
            }
        }

        if ((childpid = Fork()) == 0) { /// child process
            Close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        /* parent */
        Close(connfd);
    }
    return 0;
}


