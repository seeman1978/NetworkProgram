//
// Created by qiangwang on 2021/9/11.
//

#ifndef ICMPD_UNPICMPD_H
#define ICMPD_UNPICMPD_H
#include "../unp.h"
constexpr auto ICMPD_PATH = "/tmp/icmpd"; ///server's well-known pathname;
struct icmpd_err{
    int icmpd_errno;
    char icmpd_type;
    char icmpd_code;
    socklen_t icmpd_len;
    struct sockaddr_storage icmpd_dest;
};
#endif //ICMPD_UNPICMPD_H
