#include "../unp.h"
#include "unpicmpd.h"
#include "icmpd.h"
#include <sys/un.h>
#include <algorithm>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>

void dg_cli(FILE* fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen){
    int icmpfd, maxfdp1;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    fd_set rset;
    ssize_t n;
    struct timeval tv;
    struct icmpd_err icmpd_err;
    struct sockaddr_un sun;
    Sock_bind_wild(sockfd, pservaddr->sa_family);
    icmpfd = Socket(AF_LOCAL, SOCK_STREAM, 0);
    sun.sun_family = AF_LOCAL;
    strcpy(sun.sun_path, ICMPD_PATH);
    Connect(icmpfd, (struct sockaddr*)&sun, sizeof sun);
    Write_fd(icmpfd, (void *) "1", 1, sockfd);
    n = Read(icmpfd, recvline, 1);
    if (n!=1 || recvline[0] != '1'){
        err_quit("error creating icmp socket , n= %d , char = %c", n, recvline[0]);
    }
    FD_ZERO(&rset);
    maxfdp1 = std::max(sockfd, icmpfd) + 1;
    while (Fgets(sendline, MAXLINE, fp) != nullptr){
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        tv.tv_sec = 5;
        tv.tv_usec = 0;
        FD_SET(sockfd, &rset);
        FD_SET(icmpfd, &rset);
        if ((n= Select(maxfdp1, &rset, nullptr, nullptr, &tv)) == 0){
            fprintf(stderr, "socket timeout\n");
            continue;
        }
        if (FD_ISSET(sockfd, &rset)){
            n = Recvfrom(sockfd, recvline, MAXLINE, 0, nullptr, nullptr);
            recvline[n] = 0;
            Fputs(recvline, stdout);
        }
        if (FD_ISSET(icmpfd, &rset)){
            if ((n= Read(icmpfd, &icmpd_err, sizeof icmpd_err)) == 0){
                err_quit("ICMP daemon terminated");
            }
            else if (n!=sizeof icmpd_err){
                err_quit("n=%d, expected %d", n, sizeof icmpd_err);
            }
            printf("IMCP error:dest =%s, %s, type=%d, code=%d\n", Sock_ntop(
                           reinterpret_cast<const sockaddr *>(&icmpd_err.icmpd_dest), icmpd_err.icmpd_len),
                   strerror(icmpd_err.icmpd_errno), icmpd_err.icmpd_type, icmpd_err.icmpd_code);
        }
    }
}

int readable_listen()
{
    int i, connfd;
    socklen_t clilen;
    clilen = sizeof(cliaddr);
    connfd = Accept(listenfd, (struct sockaddr*)(&cliaddr), &clilen);

	for(i=0; i<FD_SETSIZE; ++i)
	{
		if(client[i].connfd < 0)
		{
            client[i].connfd = connfd;
			break;
		}
	}
	if(i == FD_SETSIZE)
	{
        close(connfd);
        return --nready;
	}
    printf("new connection , i=%d, connfd=%d\n", i, connfd);

    FD_SET(connfd, &allset);
	if(connfd > maxfd)
	{
        maxfd = connfd;
	}
	if(i> maxi)
	{
        maxi = i;
	}
    return --nready;
}

int readable_conn(int i)
{
    int unixfd, recvfd;
    char c;
    ssize_t n;
    socklen_t len;
    struct sockaddr_storage ss;

    unixfd = client[i].connfd;
    recvfd = -1;
	if((n=Read_fd(unixfd, &c, 1, &recvfd)) == 0)
	{
        err_msg("client %d terminated , recvfd=%d", i, recvfd);
        goto clientdone;
	}
	if(recvfd < 0)
	{
        err_msg("read_fd did not return descriptor");
        goto clienterr;
	}
    len = sizeof(ss);
	if(getsockname(recvfd, (struct sockaddr*)&ss, &len) < 0)
	{
        err_ret("getsockname error");
       goto clienterr;
	}

    client[i].family = ss.ss_family;
	if((client[i].lport = sock_get_port((struct sockaddr*)&ss, len)) == 0)
	{
        client[i].lport = sock_bind_wild(recvfd, client[i].family);
		if(client[i].lport <= 0)
		{
            err_ret("error bindingephemeral prot");
            goto clienterr;
		}
	}
    Write(unixfd, "1", 1);
    Close(recvfd);
    return --nready;

clienterr:
    Write(unixfd, "1", 1);
clientdone:
    Close(unixfd);
    if (recvfd >= 0)
    {
        Close(recvfd);
    }
    FD_CLR(unixfd, &allset);
    client[i].connfd = -1;
    return --nready;
}

int readable_v4()
{
    int i, hlen1, hlen2, icmplen, sport;
    char buf[MAXLINE];
    char srcstr[INET_ADDRSTRLEN], dststr[INET_ADDRSTRLEN];
    ssize_t n;
    socklen_t len;
    struct ip* ip, * hip;
    struct icmp* icmp;
    struct udphdr* udp;
    struct sockaddr_in from, dest;
    struct icmpd_err icmpd_err;

    len = sizeof(from);
    n = Recvfrom(fd4, buf, MAXLINE, 0, (struct sockaddr*)&from, &len);
    printf("%d bytes ICMP4 from %s:", n, Sock_ntop_host((struct sockaddr*)&from, len));
    ip = (struct ip*)buf;
    hlen1 = ip->ip_hl << 2;
    icmp = (struct icmp*)(buf + hlen1);
	if((icmplen = n-hlen1) < 8)
	{
        err_quit("icmplen (%d) < 8", icmplen);
	}
    printf(" type=%d, code=%d\n", icmp->icmp_type, icmp->icmp_code);

    if (icmp->icmp_type== ICMP_UNREACH ||
        icmp->icmp_type== ICMP_TIMXCEED ||
        icmp->icmp_type == ICMP_SOURCEQUENCH)
    {
	    if (icmplen < 8+20+8)
	    {
            err_quit("icmplen (%d)<8+20+8", icmplen);
	    }
        hip = (struct ip*)(buf + hlen1 + 8);
        hlen2 = hip->ip_hl << 2;
        printf("\tsrcip=%s, dstip=%s, proto=%d\n", Inet_ntop(AF_INET, &hip->ip_src, srcstr, sizeof srcstr),
            Inet_ntop(AF_INET, &hip->ip_dst, dststr, sizeof dststr), hip->ip_p);
	    if (hip->ip_p == IPPROTO_UDP)
	    {
            udp = (struct udphdr*)(buf + hlen1 + 8 + hlen2);
            sport = udp->uh_sport;
	    	for (i=0; i<maxi; ++i)
	    	{
	    		if(client[i].connfd >=0 &&client[i].family == AF_INET && client[i].lport == sport)
	    		{
                    bzero(&dest, sizeof dest);
                    dest.sin_family = AF_INET;
#ifdef HAVE_SOCKADDR_SA_LEN
                    dest.sin_len = sizeof(dest);
#endif
                    memcpy(&dest.sin_addr, &hip->ip_dst, sizeof(struct in_addr));
                    dest.sin_port = udp->uh_dport;

                    icmpd_err.icmpd_type = icmp->icmp_type;
                    icmpd_err.icmpd_code = icmp->icmp_code;
                    icmpd_err.icmpd_len = sizeof(struct sockaddr_in);
                    memcpy(&icmpd_err.icmpd_dest, &dest, sizeof(dest));
                    icmpd_err.icmpd_errno = EHOSTUNREACH;
                    if (icmp->icmp_type == ICMP_UNREACH)
                    {
	                    if (icmp->icmp_code == ICMP_UNREACH_PORT)
	                    {
                            icmpd_err.icmpd_errno = ECONNREFUSED;
	                    }
                        else if(icmp->icmp_code == ICMP_UNREACH_NEEDFRAG)
                        {
                            icmpd_err.icmpd_errno = EMSGSIZE;
                        }
                    }
                    Write(client[i].connfd, &icmpd_err, sizeof icmpd_err);
	    		}
	    	}
	    }
    }
    return --nready;
}


int main(int argc, char** argv) {
    int i, sockfd;
    struct sockaddr_un sun;

    if (argc != 1)
    {
        err_quit("usage : icmpd");
    }

    maxi = -1;
    for (auto element : client)
    {
        element.connfd = -1;
    }
    FD_ZERO(&allset);
    fd4 = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    FD_SET(fd4, &allset);
    maxfd = fd4;
#ifdef IPV6
    fd6 = Socket(AF_INET6, SOCK_RAW, IPPROTO_ICMP6);
    FD_SET(fd6, &allset);
    maxfd = max(maxfd, fd6);
#endif	

    listenfd = Socket(AF_UNIX, SOCK_STREAM, 0);
    sun.sun_family = AF_LOCAL;
    strcpy(sun.sun_path, ICMPD_PATH);
    unlink(ICMPD_PATH);
    Bind(listenfd, (struct sockaddr*)&sun, sizeof sun);
    Listen(listenfd, LISTENQ);
    FD_SET(listenfd, &allset);
    maxfd = maxfd(maxfd, listenfd);
	
    for(;;)
    {
        rset = allset;
        nready = Select(maxfd + 1, &rset, nullptr, nullptr, nullptr);
        if (FD_ISSET(listenfd, &rset))
        {
	        if(readable_listen() <= 0)
	        {
		        continue;
	        }
        }
        if (FD_ISSET(fd4, &rset))
        {
            if (readable_v4() <= 0)
            {
                continue;
            }
        }
#ifdef
        if(FD_ISSET(fd6, &rset))
        {
	        if(readable_v6() <= 0)
	        {
                continue;
	        }
        }
#endif
    	for (i=0; i<=maxi; ++i)
    	{
    		if((sockfd = client[i].connfd) < 0)
    		{
                continue;
    		}
    		if(FD_ISSET(sockfd, &rset))
    		{
    			if(readable_conn(i) <= 0)
    			{  
    				break;
    			}
    		}
    	} 
    }
    return 0;
}
