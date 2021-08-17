#include <iostream>
#include <fcntl.h>
#include "../../unp.h"
int main(int argc, char** argv) {
    int fd;
    if (argc != 4){
        err_quit("openfile <sockfd#> <filename> <mode>");
    }
    char * pEnd;
    if ((fd=open(argv[2], strtol(argv[3], &pEnd, 10))) < 0){
        exit((errno > 0) ? errno : 255);
    }
    char ch[1]{""};
    if (write_fd(strtol(argv[1], &pEnd, 10), ch, 1, fd) < 0){
        exit((errno>0)? errno : 255);
    }
    exit(0);
}
