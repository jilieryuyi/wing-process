
#ifndef PHP_WING_API_H
#define PHP_WING_API_H

char* get_command_path(const char* command);
int file_is_php(const char *file);
int create_process(const char *command, char* output_file);
#ifdef PHP_WIN32
#include "win32/win_api.c"
#else
#include "linux/linux_api.c"
#endif
#include "wing_api.c"

#endif