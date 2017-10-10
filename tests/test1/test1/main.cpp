//
//  main.cpp
//  test1
//
//  Created by yuyi on 17/9/17.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include <iostream>

static inline void float4store(unsigned char  *T, float  A)
{ *(T)= ((unsigned char *) &A)[3];
    *((T)+1)=(char) ((unsigned char *) &A)[2];
    *((T)+2)=(char) ((unsigned char *) &A)[1];
    *((T)+3)=(char) ((unsigned char *) &A)[0];
}

int main(int argc, const char * argv[]) {
    // insert code here...
    int a = 1|1;
    
    unsigned char t4[5];
    memset(t4, 0, 5);
    float ab= 1.001;
    float4store(t4, ab);
    
    float def_temp;
    ((unsigned char *) &def_temp)[0]=(t4)[3];
    ((unsigned char *) &def_temp)[1]=(t4)[2];
    ((unsigned char *) &def_temp)[2]=(t4)[1];
    ((unsigned char *) &def_temp)[3]=(t4)[0];
    
    
    
    //unsigned long aa= ((unsigned long) ~0);
    std::cout << def_temp << "\r\n";
    
    char *tt = (char*)t4;
    while(*tt) {
        printf("%d-", *tt++);
    }
    
    short aaa = 1;
    printf("-----%d", sizeof(aaa));
    
    
    return 0;
}
