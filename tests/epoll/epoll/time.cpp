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

/**
 * 获取时间戳
 * @return long 当前时间戳
 */
long get_timestamp()
{
    time_t t = time(NULL);
    return time(&t);
    
    /*struct timespec time;
    memset(&time, 0, sizeof(time));
    clock_gettime(CLOCK_REALTIME, &time);
    //毫秒时间戳和纳秒时间戳
    //printf("%ld----->%ld\n", time.tv_sec, time.tv_nsec);
    
    if (nsec) {
        return time.tv_nsec;
    }
    
    return time.tv_sec;*/
}

/**
 * @demo
 * char time[24];
 * get_daytime((char*)time, 24);
 * printf("current time: %s\n", time);
 * @param nowtime 用于容纳当前时间的内存指针
 * @param size nowtime的大小
 */
void get_daytime(char *nowtime, size_t size)
{
    time_t t;
    t = time(NULL);
    struct tm *lt;
    t = time(NULL);
    lt = localtime(&t);
    memset(nowtime, 0, size);
    strftime(nowtime, size, "%Y-%m-%d %H:%M:%S", lt);
}
