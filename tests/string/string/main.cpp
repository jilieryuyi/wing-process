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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "../../../win32/WString.h"
#include "../../../win32/WString.cpp"

int main(int argc, const char * argv[]) {
    std::locale::global(std::locale(""));
    setlocale(LC_CTYPE, "");    // MinGW gcc.
    std::wcout.imbue(std::locale(""));
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
    
    
    
    wchar_t zh[100];// = L"你好中国";
    std::wcsncpy(zh, L"你好中国", 100);
    
    std::wcout << L"$$$" <<zh << std::endl;
     printf("###%ls\n", zh);
    
    std::wcout << ";;;;;"<<zh;
    WString str3(zh,0,1);
    str3.append("123");
    str3.print();
    
    std::wcout <<"::::"<< wing_str_char_to_wchar("你好中国");
    
    wchar_t tt[10];
    memset(tt,0, 10*sizeof(wchar_t));
    
    memcpy((void*)tt,L"123456789000000000000",9*sizeof(wchar_t));
    //swprintf(tt, 10*sizeof(wchar_t), L"%ls" L"123456789000000000000000000000");
    std::wcout << "-----------" <<tt<<"-----------"<<std::endl;
    
    
    
    wchar_t *testss = L"asfsfd";
    wprintf(L"%ls\r\n", testss);
    
    return 0;
}
