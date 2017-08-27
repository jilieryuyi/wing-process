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
#include "wing_api.h"


#define WING_ERROR_FAILED  0
#define WING_ERROR_SUCCESS 1

static int le_wing_process;
char *PHP_PATH = NULL;

zend_class_entry *wing_process_ce;

typedef struct _WING_PROCESS_INFO {
    unsigned long process_id;
    unsigned long thread_id;
    char* command;
} WING_PROCESS_INFO;

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
	zend_string *_file = NULL;
	char *file         = NULL;
    int file_len       = 0;

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &_file)) {
        return;
    }

    if (_file) {
        file = ZSTR_VAL(_file);
        efree(_file);
    }

    file_len      = strlen(file);
    #else
    char *file    = NULL;
    int file_len  = 0;

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len)) {
        return;
    }
    #endif

	zend_update_property_string(wing_process_ce, getThis(), "file", strlen("file"), file TSRMLS_CC);

	WING_PROCESS_INFO *info = (WING_PROCESS_INFO*)emalloc(sizeof(WING_PROCESS_INFO)+1);
	info->process_id = 0;
	info->thread_id  = 0;
	info->command    = NULL;
	zend_update_property_string(wing_process_ce, getThis(), "process_info", strlen("process_info"), (unsigned long)info TSRMLS_CC);

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

		info->process_id  = (unsigned long)process_id;

		WingQueryProcessByProcessID(item, process_id);
		if (item) {
			spprintf(&command_line, size, "%s", item->command_line);
			zend_update_property_long(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), 0 TSRMLS_CC);
			delete item;
		}
		#endif
	} else {
	    #ifdef PHP_WIN32
		if (wing_file_is_php((const char*)file)){
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
	    info->command  = (char*)emalloc(strlen(command_line)+1);
	    strcpy(info->command, command_line);

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
	zval *_pi = wing_zend_read_property(wing_process_ce, getThis(),"process_info_pointer");
	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);

	if (pi) {
		CloseHandle(pi->hProcess);
		CloseHandle(pi->hThread);
		delete pi;
	}

	zval *_info = wing_zend_read_property(wing_process_ce, getThis(),"process_info");
    WING_PROCESS_INFO *info = (WING_PROCESS_INFO *)Z_LVAL_P(_info);
    if (info->command) {
        efree(info->command);
    }
	efree(info);
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
	zval *_command     = wing_zend_read_property(wing_process_ce, getThis(), "command_line");
	char *command      = Z_STRVAL_P(_command);


    #ifdef PHP_WIN32
//	STARTUPINFO sui;
	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)wing_create_process(command, output_file);//new PROCESS_INFORMATION(); // �������������ӽ��̵���Ϣ
//	SECURITY_ATTRIBUTES sa;                            // �����̴��ݸ��ӽ��̵�һЩ��Ϣ
//
//	sa.bInheritHandle       = TRUE;                         // �������ӽ��̼̳и����̵Ĺܵ����
//	sa.lpSecurityDescriptor = NULL;
//	sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
//
//	SECURITY_ATTRIBUTES *psa = NULL;
//	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
//	OSVERSIONINFO osVersion = { 0 };
//	osVersion.dwOSVersionInfoSize = sizeof(osVersion);
//	if (GetVersionEx(&osVersion)) {
//		if (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) {
//			psa = &sa;
//			dwShareMode |= FILE_SHARE_DELETE;
//		}
//	}
//
//	HANDLE hConsoleRedirect = CreateFile(
//		output_file,
//		GENERIC_WRITE,
//		dwShareMode,
//		psa,
//		OPEN_ALWAYS,
//		FILE_ATTRIBUTE_NORMAL,
//		NULL);
//	SetLastError(0);
//	ZeroMemory(&sui, sizeof(STARTUPINFO));         // ��һ���ڴ������㣬�����ZeroMemory, �����ٶ�Ҫ����memset
//
//	sui.cb         = sizeof(STARTUPINFO);
//	sui.dwFlags    = STARTF_USESTDHANDLES;
//	sui.hStdInput  = NULL;//m_hRead;
//	sui.hStdOutput = hConsoleRedirect;//m_hWrite;
//	sui.hStdError  = hConsoleRedirect;//GetStdHandle(STD_ERROR_HANDLE);
//	//sui.wShowWindow = SW_SHOW;
//	if(!redirect_output) {
//	    sui.dwFlags = STARTF_USESHOWWINDOW;// | STARTF_USESTDHANDLES;;
//	}
//									 /*if( params_len >0 ) {
//									 DWORD byteWrite  = 0;
//									 if( ::WriteFile( m_hWrite, params, params_len, &byteWrite, NULL ) == FALSE ) {
//									 php_error_docref(NULL TSRMLS_CC, E_WARNING, "write data to process error");
//									 }
//									 }*/
//	if (!CreateProcessA(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &sui, pi)) {
//		CloseHandle(hConsoleRedirect);
//		RETURN_LONG(WING_ERROR_FAILED);
//		return;
//	}
//	CloseHandle(hConsoleRedirect);

	zend_update_property_long(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), (zend_long)pi TSRMLS_CC);
	//zend_update_property_string(wing_process_ce, getThis(), "command_line", strlen("command_line"), command TSRMLS_CC);
	zend_update_property_long(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), pi->dwThreadId TSRMLS_CC);

    zval *_info = wing_zend_read_property(wing_process_ce, getThis(),"process_info");
    WING_PROCESS_INFO *info = (WING_PROCESS_INFO *)Z_LVAL_P(_info);
    info->process_id = pi->dwProcessId;
    info->thread_id  = pi->dwThreadId;
	RETURN_LONG(pi->dwProcessId);
	#else

    unsigned long childpid = wing_create_process(command, output_file);

    zval *_info = wing_zend_read_property(wing_process_ce, getThis(),"process_info");
    WING_PROCESS_INFO *info = (WING_PROCESS_INFO *)Z_LVAL_P(_info);
    info->process_id = childpid;
	RETURN_LONG(childpid);
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
	zval *file     = wing_zend_read_property(wing_process_ce, getThis(), "file");
    int process_id = 0;
	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process    = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	    process_id = zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file));
	} else {
		zval *_pi               = wing_zend_read_property(wing_process_ce, getThis(), "process_info_pointer");
		PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
		process                 = pi->hProcess;
		process_id              = pi->dwProcessId;
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

	zval *_info = wing_zend_read_property(wing_process_ce, getThis(),"process_info");
    WING_PROCESS_INFO *info = (WING_PROCESS_INFO *)Z_LVAL_P(_info);

	pid_t epid = waitpid(info->process_id, &status, timeout);
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
	zval *_info = wing_zend_read_property(wing_process_ce, getThis(),"process_info");
    WING_PROCESS_INFO *info = (WING_PROCESS_INFO *)Z_LVAL_P(_info);
    RETURN_LONG(info->process_id);
}


/**
 * 获取线程id
 *
 * @return int
 */
ZEND_METHOD(wing_process, getThreadId) {
//    #ifdef PHP_WIN32
//	zval *thread_id = wing_zend_read_property(wing_process_ce, getThis(), "thread_id");
//	RETURN_ZVAL(thread_id,0,0);
//	#else
//	RETURN_LONG(0);
//	#endif

    zval *_info = wing_zend_read_property(wing_process_ce, getThis(),"process_info");
    WING_PROCESS_INFO *info = (WING_PROCESS_INFO *)Z_LVAL_P(_info);
    RETURN_LONG(info->thread_id);
}


/**
 * 获取命令行参数
 *
 * @return string
 */
ZEND_METHOD(wing_process, getCommandLine)
{
	zval *file = wing_zend_read_property(wing_process_ce, getThis(), "file");

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
		zval *command_line  = wing_zend_read_property(wing_process_ce, getThis(), "command_line");
	    char *file          = Z_STRVAL_P(command_line);
	    char *bcommand_line = NULL;
	    if (wing_file_is_php(file) && PHP_PATH != NULL) {
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
	zval *file     = wing_zend_read_property(wing_process_ce, getThis(), "file");

	#ifdef PHP_WIN32
	HANDLE process = NULL;

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process   = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	} else {
		zval *_pi = wing_zend_read_property(wing_process_ce, getThis(), "process_info_pointer");
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
        zval *_info = wing_zend_read_property(wing_process_ce, getThis(),"process_info");
        WING_PROCESS_INFO *info = (WING_PROCESS_INFO *)Z_LVAL_P(_info);
        process_id = info->process_id;
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
	zval *file     = wing_zend_read_property(wing_process_ce, getThis(), "file");

	#ifdef PHP_WIN32
	HANDLE process = NULL;

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	} else {

		zval *_pi = wing_zend_read_property(wing_process_ce, getThis(), "process_info_pointer");
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
	ZVAL_LONG(return_value, wing_get_process_id());
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

	PHP_PATH = wing_get_command_path("php");

	REGISTER_STRING_CONSTANT("WING_PROCESS_PHP",     PHP_PATH,                 CONST_CS | CONST_PERSISTENT );
	REGISTER_STRING_CONSTANT("WING_PROCESS_VERSION", PHP_WING_PROCESS_VERSION, CONST_CS | CONST_PERSISTENT );

	zend_class_entry _wing_process_ce;
	INIT_NS_CLASS_ENTRY(_wing_process_ce, "wing", "wing_process", wing_process_methods);
	wing_process_ce = zend_register_internal_class(&_wing_process_ce TSRMLS_CC);

	zend_declare_property_string(wing_process_ce, "file", strlen("file"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_string(wing_process_ce, "output_file", strlen("output_file"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	#ifdef PHP_WIN32
	zend_declare_property_long(wing_process_ce, "process_info_pointer", strlen("process_info_pointer"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "thread_id", strlen("thread_id"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
    #endif
	zend_declare_property_string(wing_process_ce, "command_line", strlen("command_line"), "", ZEND_ACC_PRIVATE TSRMLS_CC);

	zend_declare_property_long(wing_process_ce, "process_info", strlen("process_info"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);


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

