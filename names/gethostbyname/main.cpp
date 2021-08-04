#include <iostream>
#include <netdb.h>

#include "../../unp.h"
int main() {
    std::string name{"www.baidu.com"};
    char **pptr;
    char str[MAXLINE];
    struct hostent *hptr;
    if ((hptr=gethostbyname(name.c_str())) == nullptr){
        err_msg("gethostbyname error for host : %s:%s", name.c_str(), hstrerror(h_errno));
        return 0;
    }
    std::cout << " official hostname:" << hptr->h_name << '\n';
    for (pptr = hptr->h_aliases; *pptr != nullptr; ++pptr){
        std::cout << "\talias: " << *pptr << '\n';
    }
    switch (hptr->h_addrtype) {
        case AF_INET:
        {
            for (pptr = hptr->h_addr_list; *pptr!= nullptr; ++pptr){
                std::cout << "\taddress: " << Inet_ntop(hptr->h_addrtype, *pptr, str, sizeof (str)) << '\n';
            }
            break;
        }
        default:
        {
            err_ret("unknown address type");
            break;
        }
    }
    return 0;
}
