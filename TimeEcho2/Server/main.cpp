#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXLINE 4096
#define LISTENQ 100

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main() {
    std::cout << "Hello, World!" << std::endl;
    int listenfd, connfd;
    struct sockaddr_in servaddr{}, cliaddr{};
    char buff[MAXLINE];
    time_t ticks;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);   //ip v4 tcp
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1313);

    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        handle_error("bind");
        return 0;
    }
    listen(listenfd, LISTENQ);
    for (;;) {
        socklen_t len = sizeof(cliaddr);
        connfd = accept(listenfd, (sockaddr*)&cliaddr, &len);
        std::cout << "connection from " << inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)) << " , port " << ntohs(cliaddr.sin_port) << '\n';
        ticks = time(nullptr);
        snprintf(buff, sizeof(buff), "%.24s\n", ctime(&ticks));
        write(connfd, buff, strnlen(buff, sizeof(buff)));
        close(connfd);
    }
    return 0;
}
