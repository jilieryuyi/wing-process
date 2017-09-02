//
//  main.cpp
//  string
//
//  Created by yuyi on 17/9/2.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../../../win32/WingString.class.h"
#include "../../../win32/WingString.class.cpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    char _str[] = "   123   ";
    WingString str(_str);
    printf(">%s<\r\n", str.ltrim());
    printf(">%s<\r\n", str.rtrim());
    return 0;
}
