//
//  time_test.cpp
//  epoll
//
//  Created by yuyi on 17/10/27.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include <stdio.h>
#include "time.hpp"
#include "time.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


/**
 * 时间测试
 */
void time_test()
{
    long t1 = get_timestamp();
    //long t2 = get_timestamp(1);
    printf("秒时间戳=%ld\n", t1);
    // printf("纳秒时间戳=%ld\n", t2);
    // printf("秒时间戳=%ld\n", t2/1000000);
    
    char time[24];
    get_daytime((char*)time, 24);
    printf("当前时间=%s\n", time);
}
