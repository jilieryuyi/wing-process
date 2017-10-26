//
//  time.cpp
//  epoll
//
//  Created by yuyi on 17/10/27.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include "time.hpp"
#include "time.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

long get_timestamp()
{
    time_t t = time(NULL);
    return time(&t);
    
    struct timespec time;
    memset(&time, 0, sizeof(time));
    clock_gettime(CLOCK_REALTIME, &time);
    //毫秒时间戳和纳秒时间戳
    printf("%ld----->%ld\n", time.tv_sec, time.tv_nsec);
    return 0;
}

void get_daytime(char **nowtime)
{
    time_t t;
    t = time(NULL);
    struct tm *lt;
    //long ii = time(&t);
   // printf("ii = %d\n", ii);
    t = time(NULL);
    lt = localtime(&t);
    //char nowtime[24];
    //memset(*nowtime, 0, sizeof(nowtime));
    strftime(*nowtime, 24, "%Y-%m-%d %H:%M:%S", lt);
}
