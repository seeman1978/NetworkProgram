#include <iostream>
#include "web.h"
#include "../../unp.h"
#include <netdb.h>
#include <fcntl.h>

int main(int argc, char** argv) {
    int fd, n, flags,error;
    char buf[MAXLINE];
    fd_set  rs, ws;
    if (argc < 5){
        err_quit("usage: web <#conns> <hostname> <homepage> <file1> ...");
    }
    char *end;
    const long maxnconn = strtol(argv[1], &end, 10);
    nfiles = std::min(argc-4, MAXFILES);
    for (int i = 0; i < nfiles; ++i) {
        file[i].f_name = argv[i+4];
        file[i].f_host = argv[2];
        file[i].f_flags = 0;
    }
    printf("nfiles = %d\n", nfiles);
    home_page(argv[2], argv[3]);
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    maxfd = -1;
    nlefttoread = nlefttoconn = nfiles;
    nconn = 0;
    return 0;
}

/// function prototypes
void home_page(const char* host, const char *fname){
    int fd, n;
    char line[MAXLINE];
    fd = Tcp_connect(host, SERV);   /// blocking connect()
    n = snprintf(line, sizeof line, GET_CMD, fname);
    Writen(fd, line, n);
    for (;;){
        if ((n = Read(fd, line, MAXLINE)) == 0){
            break;  /// server closed connection
        }
        printf("read %d bytes of home page\n", n);
        /// do whatever with data

    }
    printf("end of file on home page\n");
    Close(fd);
}

void start_connect(struct file* fptr){
    int fd, flags, n;
    struct addrinfo *ai;
    ai = Host_serv(fptr->f_host, SERV, 0, SOCK_STREAM);
    fd = Socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    fptr->f_fd = fd;
    printf("start connect for %s, fd %d\n", fptr->f_name, fd);
    flags = Fcntl(fd, F_GETFL, 0);
    Fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if ((n = connect(fd, ai->ai_addr, ai->ai_addrlen)) < 0){
        if (errno != EINPROGRESS){
            err_sys("nonblocking connect error");
        }
        fptr->f_flags = F_CONNECTING;
        FD_SET(fd, &rset);
        FD_SET(fd, &wset);
        if (fd > maxfd){
            maxfd = fd;
        }
    }
    else if (n >= 0){/// connect is already done
        write_get_cmd(fptr);
    }
}

void write_get_cmd(struct file* fptr){
    int n;
    char line[MAXLINE];
    n = snprintf(line, sizeof line, GET_CMD, fptr->f_name);
    Writen(fptr->f_fd, line, n);
    printf("wrote a%d bytes for %s\n", n, fptr->f_name);
    fptr->f_flags = F_READING;  ///clear F_CONNECTING
    FD_SET(fptr->f_fd, &rset);
    if (fptr->f_fd > maxfd){
        maxfd = fptr->f_fd;
    }
}
