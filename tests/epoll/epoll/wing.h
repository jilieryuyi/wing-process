//
//  wing.h
//  epoll
//
//  Created by yuyi on 17/10/24.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#ifndef wing_h
#define wing_h
#include <errno.h>
#include <string.h>

#define exit_if(r, ...) \
if (r) {\
    printf(__VA_ARGS__); \
    printf("error no: %d error msg %s\n", errno, strerror(errno)); \
    exit(1);\
}

#endif /* wing_h */
