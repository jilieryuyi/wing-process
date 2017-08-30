
#ifndef PHP_WING_API_H
#define PHP_WING_API_H

#ifdef PHP_WIN32
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_wing_process.h"
#endif

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define pid_of(pproc) pproc->kp_proc.p_pid
#endif

char* wing_get_command_path(const char* command);
int wing_file_is_php(const char *file);
unsigned long wing_create_process(const char *command, char* output_file);
int wing_get_process_id();
unsigned long wing_get_memory(int process_id);
int wing_kill(int process_id);
zval *wing_zend_read_property(zend_class_entry *scope, zval *object, const char *name);
void wing_get_tmp_dir(char *buffer);
//int wing_write_cmdline(unsigned long process_id, char *cmdline);
void wing_get_cmdline(int process_id, char **buffer);

#ifdef PHP_WIN32
#include "win32/win_api.c"
#else
#include "linux/linux_api.c"
#endif
#include "wing_api.c"
#endif