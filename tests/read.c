#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAX_PATH 255

void wing_get_tmp_dir(char *buffer)
{
    const char* tmp = "/tmp";
    if(0 != access(tmp, W_OK)) {
        buffer = NULL;
        return;
    }

    const char* tmp_wing = "/tmp/wing_process";
    if (0 == access(tmp_wing, F_OK)) {
        strcpy(buffer, tmp_wing);
        return;
    }

    if (0 == mkdir(tmp_wing, 0777)) {
        strcpy(buffer, tmp_wing);
        return;
    }
    buffer = NULL;
    return;
}
void wing_get_cmdline(unsigned long process_id, char *buffer)
{

    sprintf(buffer, "/proc/%lu/cmdline", process_id);
    if (access(buffer, F_OK) == 0) {
        //linux处理
        FILE *handle = fopen((const char*)buffer, "r");
        if (!handle) {
            buffer = NULL;
            return;
        }
        memset(buffer, 0, MAX_PATH);
        fgets(buffer, MAX_PATH, handle);
        fclose(handle);
        return;
    }
    memset(buffer, 0, MAX_PATH);
   // char tmp[MAX_PATH] = {0};
    wing_get_tmp_dir(buffer);
   // char path[MAX_PATH] = {0};
   // strcpy(path, tmp);
    strcpy((char*)(buffer+strlen(buffer)), "/");
    char _process_id[32] = {0};
    sprintf(_process_id, "%lu", process_id);
    strcpy((char*)(buffer+strlen(buffer)), _process_id);

    if (access(buffer, F_OK) != 0) {
        buffer = NULL;
        return;
    }

    strcpy((char*)(buffer+strlen(buffer)), "/cmdline");

    if (access(buffer, F_OK) != 0) {
        buffer = NULL;
        return;
    }
    FILE *handle = fopen((const char*)buffer, "r");
    if (!handle) {
        buffer = NULL;
        return;
    }
    memset(buffer, 0, MAX_PATH);
    fgets(buffer, MAX_PATH, handle);
    fclose(handle);
}


int wing_file_is_php(const char *file)
{

    char find_str[]     = " ";
    char path[MAX_PATH] = {0};

    char *find          = strstr(file, find_str);
    if (find != NULL)
    strncpy((char*)path, file, (size_t)(find-file));
    else
    strcpy((char*)path, file);

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


int main() {
    char buffer[256];
    wing_get_cmdline(973, buffer);
    printf("%s", buffer);

    printf("\r\n==%d\r\n", wing_file_is_php("/home/tools/wing-process/tests/wing_process_test.php"));
    return 0;
}