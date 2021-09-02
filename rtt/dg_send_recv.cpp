//
// Created by qiangwang on 2021/9/2.
//

#include "../unprtt.h"
#include <setjmp.h>

#define RTT_DEBUG

static struct rtt_info rttinfo;
static int rttinit = 0;
static struct msghdr msgsend, msgrecv;
static struct hdr{
    uint32_t seq;   /// sequence
    uint32_t ts;    /// timestamp when sent
} sendhdr, recvhdr;

static void sig_alrm(int signo);
static sigjmp_buf jmpbuf;

ssize_t dg_send_recv(int fd, const void *outbuff, size_t outbytes,
                     void *inbuff, size_t inbytes, const struct sockaddr *destaddr, socklen_t destlen){
    ssize_t n;
    struct iovec iovsend[2], iovrecv[2];
    if (rttinit == 0){
        rtt_init(&rttinfo); /// first time we're called
        rttinit = 1;
        rtt_d_flag = 1;
    }

    ++sendhdr.seq;
    msgsend.msg_name = destaddr;
    msgsend.msg_namelen = destlen;
    msgsend.msg_iov = iovsend;
    msgsend.msg_iovlen = 2;

    iovsend[0].iov_base = &sendhdr;
    iovsend[0].iov_len = sizeof (struct hdr);
    iovsend[1].iov_base = outbuff;
    iovsend[1].iov_len = outbytes;

    msgrecv.msg_name = nullptr;
    msgrecv.msg_namelen = 0;
    msgrecv.msg_iov = iovrecv;
    msgrecv.msg_iovlen = 2;

    iovrecv[0].iov_base = &recvhdr;
    iovrecv[0].iov_len = sizeof(struct hdr);
    iovrecv[1].iov_base = inbuff;
    iovrecv[1].iov_len = inbytes;

    Signal(SIGALRM, sig_alrm);
    rtt_newpack(&rttinfo);  /// initialize for this packet
sendagain:
    sendhdr.ts = rtt_ts(&rttinfo);
    Sendmsg(fd, &msgsend, 0);

    alarm(rtt_start(&rttinfo)); /// calc timeout value & start timer
    if (sigsetjmp(jmpbuf, 1) != 0){
        if (rtt_timeout(&rttinfo) < 0){
            err_msg("dg_send_recv: no response from server, giving up");
            rttinit = 0;
            errno = ETIMEDOUT;
            return -1;
        }
        goto sendagain;
    }
    do {
        n = Recvmsg(fd, &msgrecv, 0);
    } while (n < sizeof(struct hdr) || recvhdr.seq != sendhdr.seq);
    alarm(0);   /// stop SIGALRM timer
    rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);
    return n - sizeof(struct hdr);      ///return size of received datagram
}

static void sig_alrm(int signo){
    siglongjmp(jmpbuf, 1);
}