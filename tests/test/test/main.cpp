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
#include <sys/proc_info.h>
#include <libproc.h>
#include <sys/sysctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>

#define pid_of(pproc) pproc->kp_proc.p_pid

void wing_get_cmdline(int,char*);
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
    char path[MAX_PATH] = {0};
   
    if (strstr(file, "'") != NULL) {
        char *st = (char*)file;
        st++;
        char *f  = strstr(st, "'");
        strncpy(path, st, f - st);
    }
    
    else if (strstr(file, "\"") != NULL) {
        char *st = (char*)file;
        st++;
        char *f  = strstr(st, "\"");
        strncpy(path, st, f - st);
    }
    
    else if (strstr(file, "`") != NULL) {
        char *st = (char*)file;
        st++;
        char *f  = strstr(st, "`");
        strncpy(path, st, f - st);
    }
    
    else if(strstr(file, " ") != NULL) {
        char *st = (char*)file;
        st++;
        char *f  = strstr(st, " ");
        strncpy(path, st, f - st);
    }
    
    else {
        strcpy(path, file);
    }
    
    char *ext = strrchr(path, '.');
    if (ext) {
        ext++;
        if (strcmp(ext, "php") == 0 && access(path, R_OK) == 0) {
            return 1;
        }
    }
    
    if (access(path, R_OK) != 0) {
        return 0;
    }
    
    FILE *handle = fopen(path, "r");
    if (!handle) {
        return 0;
    }
    char *find   = NULL;
    char line[8] = { 0 };
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

void wing_get_tmp_dir(char *buffer)
{
        return ;
}
int wing_write_cmdline(unsigned long  process_id, char *cmdline)
{
    char tmp[MAX_PATH] = {0};
   // wing_get_tmp_dir(&tmp);
    char path[MAX_PATH] = {0};
    strcpy(path, tmp);
    strcpy((char*)(path+strlen(tmp)), "/");
    char _process_id[32] = {0};
    sprintf(_process_id, "%lu", process_id);
    strcpy((char*)(path+strlen(tmp)+1), _process_id);
    
    if (access(path, F_OK) != 0) {
        if (0 != mkdir(path, 0777)) {
            return 0;
        }
    }
    
    strcpy((char*)(path+strlen(tmp)+1+strlen(_process_id)), "/cmdline");
    
    FILE *handle = fopen((const char*)path, "w");
    if (handle) {
        if (1 != fwrite(cmdline, strlen(cmdline), 1, handle)) {
            fclose(handle);
            //写入失败
            return 0;
        }
        fclose(handle);
        return 1;
    }
    return 0;
}

/*extern int proc_pidinfo(int pid, int flavor, uint64_t arg, user_addr_t buffer,
                        uint32_t  buffersize);
#define SHOW_ZOMBIES 0*/

void wing_get_cmdline2(unsigned long pid, char **buffer) {
    int    mib[3], argmax, nargs, c = 0;
    size_t    size;
    char    *procargs, *sp, *np, *cp;
    int show_args = 1;
    
    //fprintf(stderr, "Getting argv of PID %d\n", pid);
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_ARGMAX;
    
    size = sizeof(argmax);
    if (sysctl(mib, 2, &argmax, &size, NULL, 0) == -1) {
        goto ERROR_A;
    }
    
    /* Allocate space for the arguments. */
    procargs = (char *)malloc(argmax);
    if (procargs == NULL) {
        goto ERROR_A;
    }
    
    
    /*
     * Make a sysctl() call to get the raw argument space of the process.
     * The layout is documented in start.s, which is part of the Csu
     * project.  In summary, it looks like:
     *
     * /---------------\ 0x00000000
     * :               :
     * :               :
     * |---------------|
     * | argc          |
     * |---------------|
     * | arg[0]        |
     * |---------------|
     * :               :
     * :               :
     * |---------------|
     * | arg[argc - 1] |
     * |---------------|
     * | 0             |
     * |---------------|
     * | env[0]        |
     * |---------------|
     * :               :
     * :               :
     * |---------------|
     * | env[n]        |
     * |---------------|
     * | 0             |
     * |---------------| <-- Beginning of data returned by sysctl() is here.
     * | argc          |
     * |---------------|
     * | exec_path     |
     * |:::::::::::::::|
     * |               |
     * | String area.  |
     * |               |
     * |---------------| <-- Top of stack.
     * :               :
     * :               :
     * \---------------/ 0xffffffff
     */
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROCARGS2;
    mib[2] = (int)pid;
    
    
    size = (size_t)argmax;
    if (sysctl(mib, 3, procargs, &size, NULL, 0) == -1) {
        goto ERROR_B;
    }
    
    memcpy(&nargs, procargs, sizeof(nargs));
    cp = procargs + sizeof(nargs);
    
    /* Skip the saved exec_path. */
    for (; cp < &procargs[size]; cp++) {
        if (*cp == '\0') {
            /* End of exec_path reached. */
            break;
        }
    }
    if (cp == &procargs[size]) {
        goto ERROR_B;
    }
    
    /* Skip trailing '\0' characters. */
    for (; cp < &procargs[size]; cp++) {
        if (*cp != '\0') {
            /* Beginning of first argument reached. */
            break;
        }
    }
    if (cp == &procargs[size]) {
        goto ERROR_B;
    }
    /* Save where the argv[0] string starts. */
    sp = cp;
    
    /*
     * Iterate through the '\0'-terminated strings and convert '\0' to ' '
     * until a string is found that has a '=' character in it (or there are
     * no more strings in procargs).  There is no way to deterministically
     * know where the command arguments end and the environment strings
     * start, which is why the '=' character is searched for as a heuristic.
     */
    for (np = NULL; c < nargs && cp < &procargs[size]; cp++) {
        if (*cp == '\0') {
            c++;
            if (np != NULL) {
                /* Convert previous '\0'. */
                *np = ' ';
            } else {
                /* *argv0len = cp - sp; */
            }
            /* Note location of current '\0'. */
            np = cp;
            
            if (!show_args) {
                /*
                 * Don't convert '\0' characters to ' '.
                 * However, we needed to know that the
                 * command name was terminated, which we
                 * now know.
                 */
                break;
            }
        }
    }
    
    /*
     * sp points to the beginning of the arguments/environment string, and
     * np should point to the '\0' terminator for the string.
     */
    if (np == NULL || np == sp) {
        /* Empty or unterminated string. */
        goto ERROR_B;
    }
    
    //size = strlen(sp)+1;
    *buffer = procargs;//(char*)malloc(size);
    
    if (*buffer == NULL) {
        goto ERROR_B;
    }
    
    memset(*buffer,0, size);
    /* Make a copy of the string. */
    printf("===>%s\r\n", sp);
    strcpy(*buffer, sp);
    
    /* Clean up. */
    //free(procargs);
    return;
    
ERROR_B:
    *buffer = NULL;
    free(procargs);
ERROR_A:
    *buffer = NULL;
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
    
//    int is = wing_file_is_php("'/Users/yuyi/phpsdk/php-7.1.8/ext/wing-process/tests/1 2.php' 123");
//    if (is == 1) {
//        std::cout << "is php file\r\n";
//    } else {
//        std::cout << "not php \r\n";
//    }
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
    
   // wing_write_cmdline(456, (char*)"php efsd.php");
    
    
    

    
    /*int numberOfProcesses = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
    pid_t pids[numberOfProcesses];
    bzero(pids, sizeof(pids));
    proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
    for (int i = 0; i < numberOfProcesses; ++i) {
        if (pids[i] == 0) { continue; }
        char pathBuffer[PROC_PIDPATHINFO_MAXSIZE];
        bzero(pathBuffer, PROC_PIDPATHINFO_MAXSIZE);
        proc_pidpath(pids[i], pathBuffer, sizeof(pathBuffer));
        if (strlen(pathBuffer) > 0) {
            printf("path: %s\n", pathBuffer);
        }
    }*/
    
//    char pathBuffer[PROC_PIDPATHINFO_MAXSIZE];
//    bzero(pathBuffer, PROC_PIDPATHINFO_MAXSIZE);
//    proc_pidpath(595, pathBuffer, sizeof(pathBuffer));
//    if (strlen(pathBuffer) > 0) {
//        printf("path: %s\n", pathBuffer);
//    }
//
//    char pathBuffer2[PROC_PIDPATHINFO_MAXSIZE];
//    bzero(pathBuffer2, PROC_PIDPATHINFO_MAXSIZE);
//    proc_name(595, pathBuffer2, sizeof(pathBuffer2));
    
   //  printf("proc_name: %s\n", pathBuffer2);
//    struct proc_taskallinfo info;
//    
//    int ret = proc_pidinfo(595, PROC_PIDTASKALLINFO, 0,
//                           (void*)&info, sizeof(struct proc_taskallinfo));
   // printf("ret=%d, result=%s---%s\n", ret, (char *) info.pbsd.pbi_comm, info.pbsd.pbi_name);
    
   // uint64_t		pti_virtual_size;	/* virtual memory size (bytes) */
   // uint64_t		pti_resident_size;	/* resident memory size (bytes) */
    //info.ptinfo
   // printf("======%llu===%llu\n", info.ptinfo.pti_virtual_size/1024/1024, info.ptinfo.pti_resident_size/1024);
    
   // proc_pidinfo(595, <#int flavor#>, uint64_t arg, <#void *buffer#>, <#int buffersize#>)
    //char buffer[1024] = {0};
    //wing_get_cmdline(595, buffer);
    
//    char *b = NULL;
//    wing_get_cmdline2((unsigned long )595, &b);
    
    
    
    
    
   /* char file_path[MAX_PATH] = {0};
    strcpy(file_path, "`234123412341324`\0");
    char *st = file_path;
    char *et = file_path+strlen(file_path);
    while(1) {
       // *st = '"';
        if (*st == '`')*st = '"';
       // printf("find:%s\r\n", st);
        st++;
        if (st > et) break;
    }
    printf("-------%s\r\n",file_path);*/
    
    
    
   // execl("/usr/local/bin/php", "php", "/Users/yuyi/phpsdk/php-7.1.8/ext/wing-process/tests/1 2.php" , "123", NULL);
    
   /* struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    
    if (KERN_SUCCESS != task_info(mach_task_self(),
                                  TASK_BASIC_INFO, (task_info_t)&t_info,
                                  &t_info_count))
    {
        return -1;
    }
    // resident size is in t_info.resident_size;
    // virtual size is in t_info.virtual_size;
    */
    
    
    const char *cmd= "'1 2 3.php'    as    '4 5 6'";
    char *st = (char*)cmd;
    char *et = (char*)(cmd + strlen(cmd));
    char _args[32][MAX_PATH];
    int pos = 0;
    int ac = 0;
    int cc = 0;
    int start = 0;
    
    int i;
    for (i=0; i<32; i++) {
        memset(*_args,0,MAX_PATH);
    }
    
    while(st <= et) {
    
        if (*st == '\'' || *st == '"' || *st == '`') {
            pos++;
            st++;
            start = 1;
        }
        
        if (start == 0) {
            if (*st == ' ') {
                ac++;
                cc = 0;
            }
        }
        
        _args[ac][cc] = *st;
        cc++;
        _args[ac][cc] = '\0';
        if (pos == 2) {
            ac++;
            cc = 0;
            pos = 0;
            while(*st == ' ')
                st++;
            printf("=======%s\r\n", st);
            start = 0;
        } else {
            st++;
        }
        
        /*
        if (pos%2 != 0) {
            _args[ac][cc] = *st;
            cc++;
            _args[ac][cc] = '\0';
        } else {
            if (*st == ' ' && start == 0) {
                while(*st == ' ')
                st++;
                ac++;
                cc = 0;
            }
            if (start) {
                ac++;
                cc = 0;
                start = 0;
            } //else {
                _args[ac][cc] = *st;
                cc++;
                _args[ac][cc] = '\0';
            //}
            
        }*/
        
        
    }
    //int i;
    for (i=0; i<32; i++) {
        printf("=>%s\r\n", _args[i]);
    }
    
    return 0;
}


void wing_get_cmdline(int pid, char *buffer) {
    int    mib[3], argmax, nargs, c = 0;
    size_t    size;
    char    *procargs, *sp, *np, *cp;
    int show_args = 1;
    
    //fprintf(stderr, "Getting argv of PID %d\n", pid);
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_ARGMAX;
    
    size = sizeof(argmax);
    if (sysctl(mib, 2, &argmax, &size, NULL, 0) == -1) {
        goto ERROR_A;
    }
    
    /* Allocate space for the arguments. */
    procargs = (char *)malloc(argmax);
    if (procargs == NULL) {
        goto ERROR_A;
    }
    
    
    /*
     * Make a sysctl() call to get the raw argument space of the process.
     * The layout is documented in start.s, which is part of the Csu
     * project.  In summary, it looks like:
     *
     * /---------------\ 0x00000000
     * :               :
     * :               :
     * |---------------|
     * | argc          |
     * |---------------|
     * | arg[0]        |
     * |---------------|
     * :               :
     * :               :
     * |---------------|
     * | arg[argc - 1] |
     * |---------------|
     * | 0             |
     * |---------------|
     * | env[0]        |
     * |---------------|
     * :               :
     * :               :
     * |---------------|
     * | env[n]        |
     * |---------------|
     * | 0             |
     * |---------------| <-- Beginning of data returned by sysctl() is here.
     * | argc          |
     * |---------------|
     * | exec_path     |
     * |:::::::::::::::|
     * |               |
     * | String area.  |
     * |               |
     * |---------------| <-- Top of stack.
     * :               :
     * :               :
     * \---------------/ 0xffffffff
     */
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROCARGS2;
    mib[2] = pid;
    
    
    size = (size_t)argmax;
    if (sysctl(mib, 3, procargs, &size, NULL, 0) == -1) {
        goto ERROR_B;
    }
    
    memcpy(&nargs, procargs, sizeof(nargs));
    cp = procargs + sizeof(nargs);
    
    /* Skip the saved exec_path. */
    for (; cp < &procargs[size]; cp++) {
        if (*cp == '\0') {
            /* End of exec_path reached. */
            break;
        }
    }
    if (cp == &procargs[size]) {
        goto ERROR_B;
    }
    
    /* Skip trailing '\0' characters. */
    for (; cp < &procargs[size]; cp++) {
        if (*cp != '\0') {
            /* Beginning of first argument reached. */
            break;
        }
    }
    if (cp == &procargs[size]) {
        goto ERROR_B;
    }
    /* Save where the argv[0] string starts. */
    sp = cp;
    
    /*
     * Iterate through the '\0'-terminated strings and convert '\0' to ' '
     * until a string is found that has a '=' character in it (or there are
     * no more strings in procargs).  There is no way to deterministically
     * know where the command arguments end and the environment strings
     * start, which is why the '=' character is searched for as a heuristic.
     */
    for (np = NULL; c < nargs && cp < &procargs[size]; cp++) {
        if (*cp == '\0') {
            c++;
            if (np != NULL) {
                /* Convert previous '\0'. */
                *np = ' ';
            } else {
                /* *argv0len = cp - sp; */
            }
            /* Note location of current '\0'. */
            np = cp;
            
            if (!show_args) {
                /*
                 * Don't convert '\0' characters to ' '.
                 * However, we needed to know that the
                 * command name was terminated, which we
                 * now know.
                 */
                break;
            }
        }
    }
    
    /*
     * sp points to the beginning of the arguments/environment string, and
     * np should point to the '\0' terminator for the string.
     */
    if (np == NULL || np == sp) {
        /* Empty or unterminated string. */
        goto ERROR_B;
    }
    
    /* Make a copy of the string. */
    printf("======%s\n", sp);
    strcpy(buffer, sp);
    printf("======%s\n", buffer);

    /* Clean up. */
    free(procargs);
    return;
    
ERROR_B:
    buffer = NULL;
    free(procargs);
ERROR_A:
    buffer = NULL;
    fprintf(stderr, "Sorry, failed\n");
    exit(2);
}
