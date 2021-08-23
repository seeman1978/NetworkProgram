#include <iostream>
#include "web.h"
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
