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
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

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
typedef unsigned long ulong;
#define MAX_PATH 256


char* get_command_path(const char* command) {
    
    char *env           = getenv("PATH");
    ulong start         = (ulong)env;
    size_t len          = strlen(env);
    ulong pos           = (ulong)env;
    ulong size          = 0;
    char temp[MAX_PATH] = {0};
    char *res           = NULL;
    ulong command_len   = strlen(command)+1;
    
    while (1) {
        char t = ((char*)start)[0];
        
        if (t == ':' ) {
            size = start - pos;
            //temp = (char *)malloc(size+command_len+1);
            memset(temp, 0, MAX_PATH);
            strncpy(temp, (char*)pos, size);
            char *base = (char*)((unsigned long)temp + strlen(temp));
            strcpy(base, "/");
            strcpy((char*)((unsigned long)base + 1), command);
            
            if (access(temp, F_OK) == 0) {
                res = (char *)malloc(size+command_len);
                memset(res, 0, size+command_len);
                strcpy(res, temp);
                return res;
            }
            
            pos = start+1;
           // free(temp);
           // temp = NULL;
        }
        
        if (start >= ((unsigned long)env+len) ) {
            break;
        }
        
        start++;
    }
    
    size = (ulong)env+len - pos;
   // temp = (char *)malloc(size+command_len+1);
    memset(temp, 0, MAX_PATH);
    strncpy(temp, (char*)pos, size);
    
    char *base = (char*)((unsigned long)temp + strlen(temp));
    strcpy(base, "/");
    strcpy((char*)((unsigned long)base + 1), command);
    
    if (access(temp, F_OK) == 0) {
        char *res = (char *)malloc(size+command_len);
        memset(res, 0, size+command_len);
        strcpy(res, temp);
        return res;
    }
    
    return NULL;
}

#define VMRSS_LINE 21//VMRSS所在行, 注:根据不同的系统,位置可能有所区别.

int get_phy_mem(const pid_t p)
{
    char file[64] = {0};//文件名
    FILE *fd;         //定义文件指针fd
    char line_buff[256] = {0};  //读取行的缓冲区
    sprintf(file,"/proc/%d/status",p);
    fprintf (stderr, "current pid:%d\n", p);
    fd = fopen (file, "r"); //以R读的方式打开文件再赋给指针fd
    
    //获取vmrss:实际物理内存占用
    int i;
    char name[32];//存放项目名称
    int vmrss;//存放内存
    //读取VmRSS这一行的数据
    for (i=0;i<VMRSS_LINE-1;i++)
    {
        char* ret = fgets (line_buff, sizeof(line_buff), fd);
    }
    char* ret1  = fgets (line_buff, sizeof(line_buff), fd);
    sscanf (line_buff, "%s %d", name,&vmrss);
    fprintf (stderr, "====%s：%d====\n", name,vmrss);
    fclose(fd);     //关闭文件fd
    return vmrss;
}

int wing_file_is_php(const char *file)
{
    
    char find_str[]     = " ";
    char *find          = strstr(file, find_str);
    char path[MAX_PATH] = {0};
    
    strncpy((char*)path, file, (size_t)(find-file));
    FILE *handle = fopen(path, "r");
    if (!handle) {
        return 0;
    }
    //char *find = NULL;
    char line[8] = { 0 };
    memset(line, 0, 8);
    fgets(line, 7, handle);
    
    find = strstr(line, "<?php");
    if (find == line) {
        fclose(handle);
        return 1;
    }
    
    memset(line, 0, 8);
    fgets(line, 7, handle);
    find = strstr(line, "<?php");
    if (find == line) {
        fclose(handle);
        return 1;
    }
    fclose(handle);
    
    return 0;
}

void init_daemon(const char* dir)
{
    int pid = fork();
    int i;
    if (pid) {
        exit(0);//是父进程，结束父进程
    }
    if (pid< 0) {
        exit(1);//fork失败，退出
    }
    //是第一子进程，后台继续执行
    setsid();//第一子进程成为新的会话组长和进程组长
    //并与控制终端分离
    pid = fork();
    if (pid) {
        exit(0);//是第一子进程，结束第一子进程
    }
    if (pid< 0) {
        exit(1);//fork失败，退出
    }
    //是第二子进程，继续
    //第二子进程不再是会话组长
    for (i = 0; i < NOFILE; ++i) {//关闭打开的文件描述符
        close(i);
    }
    chdir(dir);//改变工作目录到/tmp
    umask(0);//重设文件创建掩模 
    return; 
}

int main(int argc, const char * argv[]) {
    
    //getpid();
    // insert code here...
    /*char * php =  getCommandPath("php");
    
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
    
    */
    
    int is = wing_file_is_php("/Users/yuyi/phpsdk/php-7.1.8/ext/wing-process/tests/wing_process.php 123");
    if (is == 1) {
        std::cout << "is php file\r\n";
    } else {
        std::cout << "not php \r\n";
    }
//
//    std::cout << get_command_path("php");
//    char str[] ="/Users/yuyi/phpsdk/php-7.1.8/ext/wing-process/tests/php_path.php 123";
//    char find_str[] = "/";
//    char *find = strstr((const char*)str, find_str);
//    char *last_pos = NULL;
//    while(find) {
//        last_pos = find;
//        find++;
//        find = strstr((const char*)find, find_str);
//    }
//    char path[MAX_PATH] = {0};
//    
//   // strncpy(<#char *__dst#>, <#const char *__src#>, <#size_t __n#>)
//    strncpy((char*)path, (const char*)str, (size_t)(last_pos-str));
//    std::cout <<"===>"<< last_pos <<"\r\n";
//    std::cout <<"===>"<< (size_t)(last_pos-str) <<"---"<< path <<"\r\n";
//    
//    init_daemon((const char*)path);
    
    return 0;
}
