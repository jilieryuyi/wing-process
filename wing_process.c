/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | php windows、mac and linux daemon process support
  | Author: yuyi
  | Email: 297341015@qq.com
  | Home: http://www.itdfy.com/
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_wing_process.h"


#ifdef PHP_WIN32
#include "helper/wing_ntdll.h"
#include "Shlwapi.h"
#pragma comment(lib,"Shlwapi.lib")
#include "Psapi.h"
#pragma comment(lib,"Psapi.lib")
#else

#define INFINITE 0
#define MAX_PATH 256

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
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

#endif

#define WING_ERROR_FAILED  0
#define WING_ERROR_SUCCESS 1

/**
 * 判断是否为可执行文件，此方法一句后缀识别，不是很好
 *
 * @param char* file
 * @return BOOL
 */
//BOOL wing_check_is_runable(const char *file);

/**
 * 判断是否为php文件，此方法依据php文件开头的 <?php 识别
 *
 * @param char* file
 * @return BOOL
 */
int file_is_php(const char *file)
{
    FILE *handle = fopen(file, "r");
    if (!handle) {
        return 0;
    }
    char *find = NULL;
    char line[8] = {0};
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

static int le_wing_process;
char *PHP_PATH = NULL;

zend_class_entry *wing_process_ce;

/**
 * 构造函数，第一个参数为需要以守护进程执行的php文件或者是其他可执行的命令和文件
 * 第二个参数为重定向输出的文件路径
 * 如果传入的文件是一个数字，则认为根据进程id打开进程，后续可以对此进程进行控制，比如kill
 *
 * @param string $file
 */
ZEND_METHOD(wing_process, __construct)
{

#if PHP_MAJOR_VERSION >= 7
	zend_string *_file        = NULL;
    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
            "S", &_file)) {
            return;
        }

    char *file = NULL;
    if (_file) {
        file = ZSTR_VAL(_file);
        efree(_file);
    }

    int file_len = strlen(file);
    #else

    char *file    = NULL;
    int file_len  = 0;

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
        "s", &file, &file_len)) {
        return;
    }

    #endif

	zend_update_property_string(wing_process_ce, getThis(), "file", strlen("file"), file TSRMLS_CC);

	int size = file_len + 2;
	if (PHP_PATH) {
	    size += strlen(PHP_PATH);
	}
	char *command_line = NULL;

	//如果传入的文件是一个数字，则认为此数字为进程id
	if (is_numeric_string(file, strlen(file), NULL, NULL, 0)) {
	    #ifdef PHP_WIN32
		PROCESSINFO *item = new PROCESSINFO();
		DWORD process_id  = zend_atoi(file, strlen(file));
		WingQueryProcessByProcessID(item, process_id);
		if (item) {
			spprintf(&command_line, size, "%s", item->command_line);
			zend_update_property_long(wing_process_ce, getThis(), "process_id", strlen("process_id"), process_id TSRMLS_CC);
			zend_update_property_long(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), 0 TSRMLS_CC);
			delete item;
		}
		#endif
	} else {
	    #ifdef PHP_WIN32
		if (file_is_php((const char*)file)){
		    if (PHP_PATH) {
			    spprintf(&command_line, size, "%s %s\0", PHP_PATH, file);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "找不到php命令(could not find command php)");
			}
		} else {
			spprintf(&command_line, size, "%s\0", file);
		}
		#else
		spprintf(&command_line, size, "%s\0", file);
		#endif
	}

	if (command_line) {
		zend_update_property_string(wing_process_ce, getThis(), "command_line", strlen("command_line"), command_line TSRMLS_CC);
	    efree(command_line);
	}
}

/***
 * 析构函数
 * windows下面需要释放一些资源
 */
ZEND_METHOD(wing_process, __destruct) {
	#ifdef PHP_WIN32
	zval *_pi = zend_read_property(wing_process_ce, getThis(),
	"process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);
	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);

	if (pi) {
		CloseHandle(pi->hProcess);
		CloseHandle(pi->hThread);
		delete pi;
	}
	#endif
}

/**
 * 运行进程
 *
 * @param string $output_file 默认为null，如果不为null，则以守护进程方式运行
 * @return int 返回进程id
 */
ZEND_METHOD(wing_process, run)
{
   //php_printf(zend_get_executed_filename());
   char *output_file = NULL;

    #if PHP_MAJOR_VERSION >= 7

    zend_string *_output_file = NULL;
	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|S", &_output_file);

    if (_output_file) {
        output_file = ZSTR_VAL(_output_file);
        efree(_output_file);
    }

	#else
    int output_file_len = 0;
    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &output_file, &output_file_len);
	#endif

	int redirect_output = 1;
    if (output_file == NULL) {
        redirect_output = 0;
    }

	//zval *_output_file = zend_read_property(wing_process_ce, getThis(), "output_file", strlen("output_file"), 0, 0 TSRMLS_CC);
	//char *output_file  = Z_STRVAL_P(_output_file);
	#if PHP_MAJOR_VERSION >= 7
	zval *_command     = zend_read_property(wing_process_ce, getThis(), "command_line", strlen("command_line"), 0, 0 TSRMLS_CC);
	#else
	zval *_command     = zend_read_property(wing_process_ce, getThis(), "command_line", strlen("command_line"), 0 TSRMLS_CC);
	#endif
	char *command      = Z_STRVAL_P(_command);


    #ifdef PHP_WIN32
	STARTUPINFO sui;
	PROCESS_INFORMATION *pi = new PROCESS_INFORMATION(); // �������������ӽ��̵���Ϣ
	SECURITY_ATTRIBUTES sa;                            // �����̴��ݸ��ӽ��̵�һЩ��Ϣ

	sa.bInheritHandle       = TRUE;                         // �������ӽ��̼̳и����̵Ĺܵ����
	sa.lpSecurityDescriptor = NULL;
	sa.nLength              = sizeof(SECURITY_ATTRIBUTES);

	SECURITY_ATTRIBUTES *psa = NULL;
	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	OSVERSIONINFO osVersion = { 0 };
	osVersion.dwOSVersionInfoSize = sizeof(osVersion);
	if (GetVersionEx(&osVersion)) {
		if (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) {
			psa = &sa;
			dwShareMode |= FILE_SHARE_DELETE;
		}
	}

	HANDLE hConsoleRedirect = CreateFile(
		output_file,
		GENERIC_WRITE,
		dwShareMode,
		psa,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	SetLastError(0);
	ZeroMemory(&sui, sizeof(STARTUPINFO));         // ��һ���ڴ������㣬�����ZeroMemory, �����ٶ�Ҫ����memset

	sui.cb         = sizeof(STARTUPINFO);
	sui.dwFlags    = STARTF_USESTDHANDLES;
	sui.hStdInput  = NULL;//m_hRead;
	sui.hStdOutput = hConsoleRedirect;//m_hWrite;
	sui.hStdError  = hConsoleRedirect;//GetStdHandle(STD_ERROR_HANDLE);
	//sui.wShowWindow = SW_SHOW;
	if(!redirect_output) {
	    sui.dwFlags = STARTF_USESHOWWINDOW;// | STARTF_USESTDHANDLES;;
	}
									 /*if( params_len >0 ) {
									 DWORD byteWrite  = 0;
									 if( ::WriteFile( m_hWrite, params, params_len, &byteWrite, NULL ) == FALSE ) {
									 php_error_docref(NULL TSRMLS_CC, E_WARNING, "write data to process error");
									 }
									 }*/
	if (!CreateProcessA(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &sui, pi)) {
		CloseHandle(hConsoleRedirect);
		RETURN_LONG(WING_ERROR_FAILED);
		return;
	}
	CloseHandle(hConsoleRedirect);

	zend_update_property_long(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), (zend_long)pi TSRMLS_CC);
	//zend_update_property_string(wing_process_ce, getThis(), "command_line", strlen("command_line"), command TSRMLS_CC);
	zend_update_property_long(wing_process_ce, getThis(), "process_id", strlen("process_id"), pi->dwProcessId TSRMLS_CC);
	zend_update_property_long(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), pi->dwThreadId TSRMLS_CC);

	RETURN_LONG(pi->dwProcessId);
	#else

    if (redirect_output) {
        const char *str = zend_get_executed_filename();//"/Users/yuyi/phpsdk/php-7.1.8/ext/wing-process/tests/php_path.php";
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
        if (file_is_php(command)) {
            if (execl(PHP_PATH, "php", command ,NULL) < 0) {
                exit(0);
            }
        } else {
            if (execl("/bin/sh", "sh", "-c", command, NULL) < 0) {
                exit(0);
            }
        }
    } else if(childpid > 0) {
    	zend_update_property_long(wing_process_ce, getThis(), "process_id", strlen("process_id"), (int)childpid TSRMLS_CC);
    	if (redirect_output) {
    	    //如果以守护进程方式启动，则等待子进程退出，防止子进程变成僵尸进程
            int status;
            pid_t epid = waitpid(childpid, &status, 0);
            RETURN_LONG(epid);
        }
    } else {
         php_error_docref(NULL TSRMLS_CC, E_WARNING, "创建进程错误(fork a process error)");
    }

	RETURN_LONG((int)childpid);
	#endif
}


/**
 * 等待进程退出，直接返回退出的进程id
 *
 * @param int $timout windows下面的意思为等待超时时间，linux下面的意思为是否等待，可选参数如 WNOHANG | WUNTRACED
 * @return int
 */
ZEND_METHOD(wing_process, wait) {

	int timeout = INFINITE;
	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timeout);

    #ifdef PHP_WIN32
	HANDLE process = NULL;
	#if PHP_MAJOR_VERSION >= 7
	zval *file     = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);
    #else
    zval *file     = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0 TSRMLS_CC);
    #endif
    int process_id = 0;
	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process    = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	    process_id = zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file));
	} else {
		#if PHP_MAJOR_VERSION >= 7
		zval *_pi  = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);
        #else
		zval *_pi  = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0 TSRMLS_CC);
        #endif
		PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
		process    = pi->hProcess;
		process_id = pi->dwProcessId;
	}

	DWORD wait_result = 0;
	DWORD wait_status = WaitForSingleObject(process, timeout);

	if (wait_status != WAIT_OBJECT_0) {
		RETURN_LONG(process_id);//wait_status);
	}
	if (GetExitCodeProcess(process, &wait_result) == 0) {
		RETURN_LONG(-1);
	}

	RETURN_LONG(process_id);
	#else
	int status;
	#if PHP_MAJOR_VERSION >= 7
	zval *process_id = zend_read_property(wing_process_ce, getThis(), "process_id", strlen("process_id"), 0, 0 TSRMLS_CC);
    #else
    zval *process_id = zend_read_property(wing_process_ce, getThis(), "process_id", strlen("process_id"), 0 TSRMLS_CC);
    #endif
    pid_t childpid   = Z_LVAL_P(process_id);
	pid_t epid       = waitpid(childpid, &status, timeout);
    /*
        ret=waitpid(-1,NULL,WNOHANG | WUNTRACED);
        如果我们不想使用它们，也可以把options设为0，如：
        ret=waitpid(-1,NULL,0);
        WNOHANG 若pid指定的子进程没有结束，则waitpid()函数返回0，不予以等待。若结束，则返回该子进程的ID。
        WUNTRACED 若子进程进入暂停状态，则马上返回，但子进程的结束状态不予以理会。
        WIFSTOPPED(status)宏确定返回值是否对应与一个暂停子进程。
    */
	RETURN_LONG(epid);
	#endif
}

/**
 * 获取进程id
 *
 * @return int
 */
ZEND_METHOD(wing_process, getProcessId) {
	#if PHP_MAJOR_VERSION >= 7

	zval *process_id = zend_read_property(wing_process_ce, getThis(), "process_id", strlen("process_id"), 0, 0 TSRMLS_CC);
	#else
	zval *process_id = zend_read_property(wing_process_ce, getThis(), "process_id", strlen("process_id"), 0 TSRMLS_CC);

	#endif
	RETURN_ZVAL(process_id,0,0);
}


/**
 * 获取线程id
 *
 * @return int
 */
ZEND_METHOD(wing_process, getThreadId) {
	#if PHP_MAJOR_VERSION >= 7
	zval *thread_id = zend_read_property(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), 0, 0 TSRMLS_CC);
	#else
	zval *thread_id = zend_read_property(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), 0 TSRMLS_CC);
	#endif
	RETURN_ZVAL(thread_id,0,0);
}


/**
 * 获取命令行参数
 *
 * @return string
 */
ZEND_METHOD(wing_process, getCommandLine)
{
	#if PHP_MAJOR_VERSION >= 7

	zval *file = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);
    #else
	zval *file = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0 TSRMLS_CC);

    #endif
	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		#ifdef PHP_WIN32
		PROCESSINFO *item = new PROCESSINFO();
		WingQueryProcessByProcessID(item, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
		if (item) {
			int size = strlen(item->command_line) + 1;
			char *command_line = "";//(char*)emalloc(size);
			memset(command_line, 0, size);
			spprintf(&command_line, size,"%s", item->command_line);
			delete item;
			RETURN_STRING(command_line);
		}
		#endif
	} else {
		#if PHP_MAJOR_VERSION >= 7

		zval *command_line  = zend_read_property(wing_process_ce, getThis(),
			"command_line", strlen("command_line"), 0, 0 TSRMLS_CC);
	    #else
	    zval *command_line  = zend_read_property(wing_process_ce, getThis(),
        			"command_line", strlen("command_line"), 0 TSRMLS_CC);
	    #endif

	    char *file          = Z_STRVAL_P(command_line);
	    char *bcommand_line = NULL;
	    if (file_is_php(file) && PHP_PATH != NULL) {
	        int size = strlen(PHP_PATH) + strlen(file) + 3;
	        spprintf(&bcommand_line, size, "%s %s\0", PHP_PATH, file);
	        if (bcommand_line) {
	        #if PHP_MAJOR_VERSION >= 7
	            ZVAL_STRING(return_value, bcommand_line);
	            #else
	            ZVAL_STRING(return_value, bcommand_line, 1);
	            #endif
                efree(bcommand_line);
                bcommand_line = NULL;
	        }
	    } else {
		    ZVAL_ZVAL(return_value, command_line, 0, 0);
		}
	}
}

/**
 * 杀死进程
 *
 * @return int
 */
ZEND_METHOD(wing_process, kill)
{

	#if PHP_MAJOR_VERSION >= 7

	zval *file     = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);
	#else
	zval *file     = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0 TSRMLS_CC);

	#endif
	#ifdef PHP_WIN32
	HANDLE process = NULL;

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process   = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	} else {
		#if PHP_MAJOR_VERSION >= 7
		zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);
        #else
		zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0 TSRMLS_CC);

        #endif
		PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
		process   = pi->hProcess;
	}
    //非安全的方式直接退出 可能造成进程数据丢失
	if (!TerminateProcess(process, 0)) {
		RETURN_FALSE;//LONG(WING_ERROR_FAILED);
		return;
	}
	//RETURN_LONG(WING_ERROR_SUCCESS);
	RETURN_TRUE;
	#else
    int process_id = 0;
    if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
        process_id = zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file));
    } else {
    	#if PHP_MAJOR_VERSION >= 7
        zval *_process_id = zend_read_property(wing_process_ce, getThis(), "process_id", strlen("process_id"), 0, 0 TSRMLS_CC);
        #else
        zval *_process_id = zend_read_property(wing_process_ce, getThis(), "process_id", strlen("process_id"), 0 TSRMLS_CC);
        #endif
        process_id = Z_LVAL_P(_process_id);
    }
    int status = kill(process_id, SIGKILL);
    if (status == -1) {
        RETURN_FALSE;
    }
    wait(&status);
    if (WIFSIGNALED(status)) {
        //printf("chile process receive signal %d\n",WTERMSIG(status));
        RETURN_TRUE;
    }
    RETURN_FALSE;
	#endif
}

/**
 * 返回进程占用的实际内存，单位为字节
 *
 * @return int
 */
ZEND_METHOD(wing_process, getMemory) {
    #if PHP_MAJOR_VERSION >= 7
	zval *file     = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);
	#else
	zval *file     = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0 TSRMLS_CC);
	#endif

	#ifdef PHP_WIN32
	HANDLE process = NULL;

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	} else {
	    #if PHP_MAJOR_VERSION >= 7

		zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);
        #else
		zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0 TSRMLS_CC);

        #endif
		PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
		process = pi->hProcess;
	}

	if (!process) {
		RETURN_LONG(0);
	}

	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(process, &pmc, sizeof(pmc));
	RETURN_LONG(pmc.WorkingSetSize);
	#endif
}

/**
 * 静态方法，获取当前进程id
 *
 * @return int
 */
ZEND_METHOD(wing_process, getCurrentProcessId) {
    #ifdef PHP_WIN32
	ZVAL_LONG(return_value, GetCurrentProcessId());
	#else
	ZVAL_LONG(return_value, getpid());
	#endif
}


static zend_function_entry wing_process_methods[] = {
	ZEND_ME(wing_process, __construct, NULL,ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_ME(wing_process, __destruct, NULL,ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	ZEND_ME(wing_process, wait, NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, run, NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getProcessId, NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getThreadId, NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getCommandLine, NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, kill, NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getMemory, NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getCurrentProcessId, NULL,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{
	    NULL,NULL,NULL
	}
};


PHP_MINIT_FUNCTION(wing_process)
{

    #ifdef PHP_WIN32
	PHP_PATH = (char*)malloc(MAX_PATH);
	memset(PHP_PATH, 0, MAX_PATH);
	GetModuleFileName(NULL, PHP_PATH, MAX_PATH);
	#else
	PHP_PATH = get_command_path("php");
	#endif

	REGISTER_STRING_CONSTANT("WING_PROCESS_PHP",     PHP_PATH,                 CONST_CS | CONST_PERSISTENT );
	REGISTER_STRING_CONSTANT("WING_PROCESS_VERSION", PHP_WING_PROCESS_VERSION, CONST_CS | CONST_PERSISTENT );

	zend_class_entry _wing_process_ce;
	INIT_NS_CLASS_ENTRY(_wing_process_ce, "wing", "wing_process", wing_process_methods);
	wing_process_ce = zend_register_internal_class(&_wing_process_ce TSRMLS_CC);

	zend_declare_property_string(wing_process_ce, "file", strlen("file"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_string(wing_process_ce, "output_file", strlen("output_file"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "process_info_pointer", strlen("process_info_pointer"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_string(wing_process_ce, "command_line", strlen("command_line"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "process_id", strlen("process_id"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "thread_id", strlen("thread_id"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);


	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(wing_process)
{

	if (PHP_PATH) {
		free(PHP_PATH);
	}
	return SUCCESS;
}

PHP_RINIT_FUNCTION(wing_process)
{
#if PHP_MAJOR_VERSION >= 7
#if defined(COMPILE_DL_WING_PROCESS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
#endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(wing_process)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(wing_process)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "wing_process support", "enabled");
	php_info_print_table_end();
}


const zend_function_entry wing_process_functions[] = {
//	PHP_FE(wing_process_wait,NULL)
//	PHP_FE(wing_create_process_ex,NULL)
	//PHP_FE(alarm, NULL)
	PHP_FE_END	/* Must be the last line in wing_process_functions[] */
};

zend_module_entry wing_process_module_entry = {
	STANDARD_MODULE_HEADER,
	"wing_process",
	wing_process_functions,
	PHP_MINIT(wing_process),
	PHP_MSHUTDOWN(wing_process),
	PHP_RINIT(wing_process),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(wing_process),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(wing_process),
	PHP_WING_PROCESS_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_WING_PROCESS
#if PHP_MAJOR_VERSION >= 7
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
#endif
ZEND_GET_MODULE(wing_process)
#endif

