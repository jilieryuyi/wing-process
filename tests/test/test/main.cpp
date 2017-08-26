//
//  main.cpp
//  test
//
//  Created by yuyi on 17/8/24.
//  Copyright © 2017年 yuyi. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <unistd.h>

//http://www.jb51.net/article/45012.htm c linux 根据进程id获取进程信息
/**
 * 查找命令所在路径，使用完需要free释放资源
 */
char* getCommandPath(const char* command) {

    char *env = getenv("PATH");
    unsigned long start = (unsigned long)env;
    size_t len = strlen(env);
    unsigned long pos = (unsigned long)env;
    unsigned long size = 0;
    char *temp = NULL;
    unsigned long command_len = strlen(command)+1;
    
    while(1) {
        char t = ((char*)start)[0];
        
        if (t == ':' ) {
            size = start - pos;
            temp = (char *)malloc(size+command_len+1);
            memset(temp, 0, size+command_len+1);
            strncpy(temp, (char*)pos, size);
            char *base = (char*)((unsigned long)temp + strlen(temp));
            strcpy(base, "/");
            strcpy((char*)((unsigned long)base + 1), command);
            
            std::cout << temp << "\r\n";
            if (access(temp, F_OK) == 0) {
                std::cout << command << " path is : " << temp << "\r\n";
                return temp;
            }
            
            pos = start+1;
            free(temp);
            temp = NULL;
        }
        
        if (start >= ((unsigned long)env+len) ) {
            break;
        }
        
        start++;
    }
    
    
    
    size = (unsigned long)env+len - pos;
    
    temp = (char *)malloc(size+command_len+1);
    memset(temp, 0, size+command_len+1);
    strncpy(temp, (char*)pos, size);
   
    char *base = (char*)((unsigned long)temp + strlen(temp));
    strcpy(base, "/");
    strcpy((char*)((unsigned long)base + 1), command);
    
    std::cout << temp << "\r\n";
    if (access(temp, F_OK) == 0) {
        std::cout << command << " path is : " << temp << "\r\n";
        return temp;
    }
    free(temp);
    temp = NULL;
    
    
    return NULL;

}

int main(int argc, const char * argv[]) {
    // insert code here...
    char * php =  getCommandPath("php");
    
    std::cout << "php path is => " << php << "\r\n";
    free(php);
    
    FILE *handle = fopen("/Users/yuyi/phpsdk/php-7.1.8/ext/wing-process/tests//php_path.php", "r");
    char *line1 = (char*)malloc(8);
    memset(line1, 0 , 7);
    fgets(line1, 7, handle);
    std::cout << line1 << "\r\n";
    char *find =strstr(line1, "<?php");
    if(find == line1 ) {
        std::cout << "line1是php文件\r\n";
    }
    
    char *line2 = (char*)malloc(8);
    memset(line2, 0 , 7);
    fgets(line2, 7, handle);
    std::cout << line2 << "\r\n";
    char *find2 =strstr(line2, "<?php");
    if(find2 == line2 ) {
        std::cout << "line2是php文件\r\n";
    }
    fclose(handle);
    free(line1);
    free(line2);
    return 0;
}
