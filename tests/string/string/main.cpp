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
#include "../../../win32/WString.h"
#include "../../../win32/WString.cpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    WString str1;
    str1.append("123");
    str1.print();
    
    WString str2("123",0,1);
    str2.append("456");
    str2.print();
    
    str1.append(str2);
    str1.print();
    
    char *_str = " 123456789 ";
    WString str(_str, 0, 1);
    printf("length:%zu\r\n", str.length());
//    printf(">%s<\r\n", str.rtrim());
//    printf("length:%zu\r\n", str.length());
//    printf(">%s<\r\n", str.ltrim());
//    printf("length:%zu\r\n", str.length());
    printf(">%s<\r\n", str.trim());
    printf("length:%zu\r\n", str.length());
    
    
    str.append("0001", 4);
    str.print();
    printf("length:%zu\r\n", str.length());
    //printf(">%s<\r\n", str.substr(2, 2));
    return 0;
}
