#include "linux_api.h"
/**
 * linux或者mac查找命令所在路径，使用完需要free释放资源
 * 如：get_command_path("php"); //返回 /usr/bin/php
 */
char* get_command_path(const char* command)
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