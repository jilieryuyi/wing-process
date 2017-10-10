
#ifndef PHP_WING_API_H
#define PHP_WING_API_H

#ifdef PHP_WIN32
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_wing_process.h"
#endif

#define MAX_ARGC  8

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define pid_of(pproc) pproc->kp_proc.p_pid
#include <libproc.h>
#endif

char* wing_get_command_path(const char* command);
int wing_file_is_php(const char *file);
unsigned long wing_create_process(const char *command, char* output_file);
int wing_get_process_id();
unsigned long wing_get_memory(int process_id);
int wing_kill(int process_id);
zval *wing_zend_read_property(zend_class_entry *scope, zval *object, const char *name);
void wing_get_tmp_dir(char *buffer);
void wing_get_cmdline(int process_id, char **buffer);

/**
 * 判断文件是否存在、是否可读、可写等等
 * @param const char* file 文件路径
 * @param int mode 可直接使用系统定义的常量，可以组合使用，比如判断是否可读并且可写，传入 R_OK|W_OK
     在头文件unistd.h中的预定义如下：
     #define R_OK 4
     #define W_OK 2
     #define X_OK 1
     #define F_OK 0
     具体含义如下：
     R_OK 只判断是否有读权限
     W_OK 只判断是否有写权限
     X_OK 判断是否有执行权限
     F_OK 只判断是否存在
     在宏定义里面分别对应：
     00 只存在
     02 写权限
     04 读权限
     06 读和写权限
 * @return int 0代表成功 -1代表失败，使用的时候判断 == 0 或者 == -1 、!= 0 即可
 */
int wing_access(const char* file, int mode)
{
#ifdef PHP_WIN32
	return _access(file, mode);
#else 
	return access(file, mode);
#endif
}

#ifdef PHP_WIN32
#include "win32/win_api.c"
#else
#include "linux/linux_api.c"
#endif
#include "wing_api.c"
#endif