//
//  main.cpp
//  test
//
//  Created by yuyi on 17/8/24.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include <iostream>
#include <string.h>

BOOL wing_check_is_runable(const char *file) {
    
    char *begin = NULL;
    char *find = NULL;
    
    if (file[0] == '\'' || file[0] == '\"') {
        begin = (char*)(file + 1);
    }
    else {
        begin = (char*)file;
    }
    
    
    find = strchr(begin, '.');
    if (!find)
    {
        return 0;
    }
    const char *p = strchr(begin, '.') + 1;
    
    char *ext = (char*)emalloc(4);
    memset(ext, 0, 4);
    strncpy(ext, 4, p, 3);
    
    BOOL is_run = 0;
    if (strcmp(ext, "exe") == 0 || strcmp(ext, "bat") == 0)
    {
        is_run = 1;
    }
    efree(ext);
    return is_run;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
