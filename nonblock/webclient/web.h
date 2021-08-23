//
// Created by qiangwang on 2021/8/20.
//

#ifndef WEBCLIENT_WEB_H
#define WEBCLIENT_WEB_H

#include "../../unp.h"
#define MAXFILES 20
#define SERV "80"

struct file{
    char *f_name;   /// file name
    char *f_host;   /// hostname or IPv4/IPv6 address
    int f_fd;   /// descriptor
    int f_flags;    /// F_xxx below
} file[MAXFILES];

#define F_CONNECTING 1     /// connect() in progress
#define F_READING    2     /// connect() complete ; now reading
#define F_DONE       4     /// all done

#define GET_CMD "GET %s HTTP/1.0\r\n\r\n"

/// global
int nconn/* 当前打开着的连接数*/, nfiles, nlefttoconn/*尚无tcp连接的文件数*/,
    nlefttoread /*仍待读取的文件数（当它到达0是程序任务完成）*/, maxfd;
fd_set rset, wset;

/// function prototypes
void home_page(const char* host, const char *fname);
void start_connect(struct file* fptr);
void write_get_cmd(struct file*);

#endif //WEBCLIENT_WEB_H
