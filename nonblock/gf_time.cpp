//
// Created by qiangwang on 2021/8/18.
//

#include "../unp.h"
#include <sys/time.h>
#include <ctime>
#include <cstring>

char* gf_time(){
    struct timeval tv;
    static char str[30];
    char *ptr;
    if (gettimeofday(&tv, nullptr) < 0){
        err_sys("gettimeofday error");
    }
    ptr = ctime(&tv.tv_sec);
    strcpy(str, &ptr[11]);

    snprintf(str+8, sizeof(str)-8, ".%06ld", tv.tv_usec );
    return str;
}
