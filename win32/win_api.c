#include "win_api.h"

char* wing_get_command_path(const char* command)
{
    char *PHP_PATH = (char*)malloc(MAX_PATH);
    memset(PHP_PATH, 0, MAX_PATH);
    GetModuleFileName(NULL, PHP_PATH, MAX_PATH);

    return PHP_PATH;
}

int wing_get_process_id()
{
    return GetCurrentProcessId();
}

unsigned long get_memory(int process_id)
{
    return 0;
}

int wing_kill(int process_id)
{
    return 0;
}