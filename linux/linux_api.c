#include "linux_api.h"
extern char* PHP_PATH;
/**
 * linux或者mac查找命令所在路径，使用完需要free释放资源
 * 如：get_command_path("php"); //返回 /usr/bin/php
 */
char* wing_get_command_path(const char* command)
{

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
        }

        if (start >= ((unsigned long)env+len) ) {
            break;
        }

        start++;
    }

    size = (ulong)env+len - pos;
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
    return NULL;
}


void init_daemon(const char* dir)
{
    int pid = fork();
    int i;
    if (pid > 0) {
        exit(0);//是父进程，结束父进程
    }
    if (pid < 0) {
        exit(1);//fork失败，退出
    }
    //是第一子进程，后台继续执行
    setsid();//第一子进程成为新的会话组长和进程组长
    //并与控制终端分离
    pid = fork();
    if (pid > 0) {
        exit(0);//是第一子进程，结束第一子进程
    }
    if (pid < 0) {
        exit(1);//fork失败，退出
    }
    //是第二子进程，继续
    //第二子进程不再是会话组长
//    for (i = 0; i < NOFILE; ++i) {//关闭打开的文件描述符
//        close(i);
//    }
    chdir(dir);//改变工作目录到/tmp
    umask(0);//重设文件创建掩模
    return;
}

unsigned long wing_create_process(const char *command, char* output_file)
{
    int daemon = output_file == NULL ? 0 : 1;
    if (daemon) {
    //    #if PHP_MAJOR_VERSION >= 7
    //    const char *str = zend_get_executed_filename();
    //    #else
        const char *str = zend_get_executed_filename(TSRMLS_C);
        //#endif
        char find_str[] = "/";
        char *find      = strstr((const char*)str, find_str);
        char *last_pos  = NULL;
        while(find) {
            last_pos = find;
            find++;
            find = strstr((const char*)find, find_str);
        }
        char path[MAX_PATH] = {0};

        strncpy((char*)path, (const char*)str, (size_t)(last_pos-str));

        init_daemon((const char*)path);

        FILE *handle = fopen(output_file, "a+");

        if (handle) {
            fclose(handle);
            fclose((FILE*)stdout);
            stdout = fopen(output_file, "a+");

            fclose((FILE*)stderr);
            stderr = fopen(output_file, "a+");
        } else {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "无法打开文件(could not open file)：%s", output_file);
        }

    }
    pid_t childpid = fork();

    if (childpid == 0) {
        if (wing_file_is_php(command)) {
            if (execl(PHP_PATH, "php", command ,NULL) < 0) {
                exit(0);
            }
        } else {
            if (execl("/bin/sh", "sh", "-c", command, NULL) < 0) {
                exit(0);
            }
        }
    } else if(childpid > 0) {
        if (daemon) {
            //如果以守护进程方式启动，则等待子进程退出，防止子进程变成僵尸进程
            int status;
            return waitpid(childpid, &status, 0);
        }
    } else {
         php_error_docref(NULL TSRMLS_CC, E_WARNING, "创建进程错误(fork a process error)");
    }
    return (unsigned long )childpid;
}

int wing_get_process_id()
{
    return getpid();
}

unsigned long get_memory(int process_id)
{
    return 0;
}

int wing_kill(int process_id)
{
    return 0;
}