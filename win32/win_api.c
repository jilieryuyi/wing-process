#include "win_api.h"

char* get_command_path(const char* command)
{
    char *PHP_PATH = (char*)malloc(MAX_PATH);
    memset(PHP_PATH, 0, MAX_PATH);
    GetModuleFileName(NULL, PHP_PATH, MAX_PATH);

    return PHP_PATH;
}

int get_process_id()
{
    return GetCurrentProcessId();
}