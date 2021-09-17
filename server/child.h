//
// Created by qiangwang on 2021/9/17.
//

#ifndef NETWORKPROGRAM_CHILD_H
#define NETWORKPROGRAM_CHILD_H

typedef struct {
    pid_t child_pid;    ///process ID of child process
    int child_pipefd;   ///parent's stream pipe to/from child
    int child_status;   ///0=ready of child process
    long child_count;   /// connections handled of child process
} Child;

extern Child *cptr;    ///array of Child structures

#endif //NETWORKPROGRAM_CHILD_H
