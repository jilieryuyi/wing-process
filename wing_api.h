
#ifndef PHP_WING_API_H
#define PHP_WING_API_H

char* wing_get_command_path(const char* command);
int wing_file_is_php(const char *file);
unsigned long wing_create_process(const char *command, char* output_file);
int wing_get_process_id();
unsigned long get_memory(int process_id);
int wing_kill(int process_id);

ZEND_API zval *wing_zend_read_property(zend_class_entry *scope, zval *object, const char *name);

#ifdef PHP_WIN32
#include "win32/win_api.c"
#else
#include "linux/linux_api.c"
#endif
#include "wing_api.c"

#endif