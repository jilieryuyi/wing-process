#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>  
#define MAX_PATH 255
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

int main() {
    char buffer[256];
    wing_get_cmdline(973, buffer);
    printf("%s", buffer);
    return 0;
}