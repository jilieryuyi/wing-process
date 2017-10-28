//
//  wing.h
//  epoll
//
//  Created by yuyi on 17/10/24.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#ifndef wing_h
#define wing_h
#include "errno.h"
#include "time.hpp"
#include <stdlib.h>


#define exit_if(r, ...) \
if (r) {\
    char time[24];\
    get_daytime((char*)time, 24);\
    printf("%s ", time);\
    printf(__VA_ARGS__); \
    printf("  error no: %d error msg %s\n", errno, strerror(errno)); \
    exit(1);\
}

#define SEND_QUEUE_BUF_SIZE 10240 //队列最大容量
#define MAX_SEND_TIMES 100        //发送失败最大的重试次数

#define debug(...) \
{\
    char time[24];\
    get_daytime((char*)time, 24);\
    printf("%s ", time);\
    printf(__VA_ARGS__);\
    if (errno) \
    printf("\nerror happened: %d %s\n", errno, strerror(errno)); \
    printf("\n");\
}

#endif /* wing_h */
