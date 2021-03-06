//
// Created by qiangwang on 2021/9/15.
//连接在子进程中的分布
#include "../unp.h"
#include <sys/mman.h>

long* meter(int nchildren){
    int fd;
    long *ptr;
#ifdef MAP_ANON
    ptr = static_cast<long *>(Mmap(0, nchildren * sizeof(long), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0));
#else
    fd = Open("/dev/zero", O_RDWR, 0);
    ptr = Mmap(0, nchildren*sizeof(long), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    Close(fd);
#endif
    return ptr;
}

