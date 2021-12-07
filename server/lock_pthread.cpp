//
// Created by qiangwang on 2021/9/16.
//

#include "../unpthread.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <thread>

static pthread_mutex_t  *mptr;

void my_lock_init(){
    int fd;
    pthread_mutexattr_t  mattr;
    fd = Open("/dev/zero", O_RDWR, 0);
    mptr = static_cast<pthread_mutex_t *>(Mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    Close(fd);

    Pthread_mutexattr_init(&mattr);
    Pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    Pthread_mutex_init(mptr, &mattr);
}

void my_lock_wait(){
    Pthread_mutex_lock(mptr);
}

void my_lock_release(){
    Pthread_mutex_unlock(mptr);
}