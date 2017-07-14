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
  | php win32 deamon process support
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

#include "helper/wing_ntdll.h"
#include "Shlwapi.h"
#pragma comment(lib,"Shlwapi.lib")
#include "Psapi.h"
#pragma comment(lib,"Psapi.lib")

#define WING_ERROR_FAILED  0
#define WING_ERROR_SUCCESS 1


BOOL wing_check_is_runable(const char *file);

static int le_wing_process;
char *PHP_PATH = NULL;

zend_class_entry *wing_process_ce;

/**
 * 检查文件是否为可执行文件
 * 
 * @param string 文件名（可以带路径，也可以不带路径）
 * @return bool
 */
BOOL wing_check_is_runable(const char *file) {

	char *begin = NULL;
	char *find = NULL;

	if (file[0] == '\'' || file[0] == '\"') {
		begin = (char*)(file + 1);
	}
	else {
		begin = (char*)file;
	}


	find = strchr(begin, '.');
	if (!find)
	{
		return 0;
	}
	const char *p = strchr(begin, '.') + 1;

	char *ext = (char*)emalloc(4);
	memset(ext, 0, 4);
	strncpy_s(ext, 4, p, 3);

	BOOL is_run = 0;
	if (strcmp(ext, "exe") == 0 || strcmp(ext, "bat") == 0)
	{
		is_run = 1;
	}
	efree(ext);
	return is_run;
}

/**
 * 构造函数
 * @param $file
 * @param $ouput
 */
ZEND_METHOD(wing_process, __construct) 
{
	
	char *file        = NULL;  
	char *output_file = "";
	int file_len      = 0;
	int output_len    = 0;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, 
		"s|s", &file, &file_len, &output_file, &output_len)) {
		return;
	}

	zend_update_property_string( wing_process_ce, getThis(), 
		"file", strlen("file"), file TSRMLS_CC );
	zend_update_property_string( wing_process_ce, getThis(), 
		"output_file", strlen("output_file"), output_file TSRMLS_CC );

	int size = strlen(PHP_PATH) + file_len + 2;
	char *command_line = NULL;

	//如果参数是数字 则认为需要通过已有进程id创建进程
	if (is_numeric_string(file, strlen(file), NULL, NULL, 0)) {
		PROCESSINFO *item = new PROCESSINFO();
		DWORD process_id  = zend_atoi(file, strlen(file));
		WingQueryProcessByProcessID(item, process_id);
		if (item){
			spprintf(&command_line, size, "%s", item->command_line);
			zend_update_property_long(wing_process_ce, getThis(), "process_id", strlen("process_id"), process_id TSRMLS_CC);
			zend_update_property_long(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), 0 TSRMLS_CC);
			delete item;
		}
	} else {
		if (!wing_check_is_runable((const char*)file)){
			spprintf(&command_line,size,"%s %s\0", PHP_PATH, file);
		} else {
			spprintf(&command_line, size, "%s\0", file);
		}
	}

	zend_update_property_string(wing_process_ce, getThis(), "command_line", strlen("command_line"), command_line TSRMLS_CC);
	if(command_line)
	efree(command_line);
}

/***
 * 析构函数
 */
ZEND_METHOD(wing_process, __destruct) {
	 
	zval *_pi = zend_read_property(wing_process_ce, getThis(),
		"process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);

	if (pi) {
		CloseHandle(pi->hProcess);
		CloseHandle(pi->hThread);
		delete pi;
	}
}

/**
 * 开始执行
 *
 * @param int 是否重定向输出，可以理解为，如果重定向输出即为守护进程，默认是1 重定向输出
 */
ZEND_METHOD(wing_process, run) 
{
	int redirect_output = 1;
	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &redirect_output);
	
	zval *_output_file = zend_read_property(wing_process_ce, getThis(), "output_file", strlen("output_file"), 0, 0 TSRMLS_CC);
	char *output_file  = Z_STRVAL_P(_output_file);
	zval *_command     = zend_read_property(wing_process_ce, getThis(), "command_line", strlen("command_line"), 0, 0 TSRMLS_CC);
	char *command      = Z_STRVAL_P(_command);


	STARTUPINFO sui;
	PROCESS_INFORMATION *pi=new PROCESS_INFORMATION(); // 保存了所创建子进程的信息
	SECURITY_ATTRIBUTES sa;                            // 父进程传递给子进程的一些信息



	sa.bInheritHandle = TRUE;                         // 来允许子进程继承父进程的管道句柄
	sa.lpSecurityDescriptor = NULL;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);

	SECURITY_ATTRIBUTES *psa = NULL;
	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	OSVERSIONINFO osVersion = { 0 };
	osVersion.dwOSVersionInfoSize = sizeof(osVersion);
	if (GetVersionEx(&osVersion))
	{
		if (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
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
	ZeroMemory(&sui, sizeof(STARTUPINFO));         // 对一个内存区清零，最好用ZeroMemory, 它的速度要快于memset

	sui.cb = sizeof(STARTUPINFO);
	sui.dwFlags = STARTF_USESTDHANDLES;
	sui.hStdInput = NULL;//m_hRead;
	sui.hStdOutput = hConsoleRedirect;//m_hWrite;
	sui.hStdError = hConsoleRedirect;//GetStdHandle(STD_ERROR_HANDLE);
	//sui.wShowWindow = SW_SHOW;
	if(!redirect_output)
	sui.dwFlags = STARTF_USESHOWWINDOW;// | STARTF_USESTDHANDLES;;
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
}


/**
 * 等待进程结束
 * 
 * @param int $timout 等待多少秒超时，可选参数，默认为 INFINITE， 意思为永不超时
 * @return int
 */
ZEND_METHOD(wing_process, wait) {
	
	int timeout = INFINITE;

	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timeout);

	HANDLE process = NULL;
	zval *file = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	}
	else {
		zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

		PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
		process = pi->hProcess;
	}

	DWORD wait_result = 0;
	DWORD wait_status = WaitForSingleObject(process, timeout);

	if (wait_status != WAIT_OBJECT_0) {
		RETURN_LONG(wait_status);
	}
	if (GetExitCodeProcess(process, &wait_result) == 0) {
		RETURN_LONG(WING_ERROR_FAILED);
	}

	RETURN_LONG(wait_result);
}

/**
 * 获取进程id
 *
 * @return int
 */
ZEND_METHOD(wing_process, getProcessId) {
	zval *process_id = zend_read_property(wing_process_ce, getThis(), "process_id", strlen("process_id"), 0, 0 TSRMLS_CC);
	RETURN_ZVAL(process_id,0,0);
}


/**
 * 获取线程id
 *
 * @return int
 */
ZEND_METHOD(wing_process, getThreadId) {
	zval *thread_id = zend_read_property(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), 0, 0 TSRMLS_CC);
	RETURN_ZVAL(thread_id,0,0);
}


/**
 * 获取进程启动参数
 *
 * @return string
 */
ZEND_METHOD(wing_process, getCommandLine)
{
	zval *file = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		PROCESSINFO *item = new PROCESSINFO();
		WingQueryProcessByProcessID(item, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
		if (item)
		{
			int size = strlen(item->command_line) + 1;
			char *command_line = "";//(char*)emalloc(size);
			memset(command_line, 0, size);
			spprintf(&command_line, size,"%s", item->command_line);
			delete item;
			RETURN_STRING(command_line);
		}
	}
	else {

		zval *command_line = zend_read_property(wing_process_ce, getThis(),
			"command_line", strlen("command_line"), 0, 0 TSRMLS_CC);
	
		ZVAL_ZVAL(return_value, command_line, 0, 0);
	}
}

/**
 * 杀死进程
 *
 * @return int
 */
ZEND_METHOD(wing_process, kill)
{

	zval *file = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);
	HANDLE process = NULL;

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	}
	else {
		zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

		PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
		process = pi->hProcess;
	}

	if (!TerminateProcess(process, 0)) {

		RETURN_LONG(WING_ERROR_FAILED);
		return;
	}
	RETURN_LONG(WING_ERROR_SUCCESS);
}

ZEND_METHOD(wing_process, getMemory) {

	zval *file = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);
	HANDLE process = NULL;

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
	}
	else {
		zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

		PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
		process = pi->hProcess;
	}

	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(process, &pmc, sizeof(pmc));
	RETURN_LONG(pmc.WorkingSetSize);
}

ZEND_METHOD(wing_process, getCurrentProcessId) {
	ZVAL_LONG(return_value, GetCurrentProcessId());
}


jmp_buf j;
void raise_exception(void)
{
	printf("exception raised\n");
	longjmp(j, 1); /* jump to exception handler */
	printf("this line should never appear\n");
}


DWORD WINAPI ThreadFuncFirst(LPVOID param)
{
	printf("thread is running\r\n");
	//Sleep(1000000);
	printf("exception raised\n");
	longjmp(j, 1); /* jump to exception handler */
	printf("this line should never appear\n");
	return 0;
}

ZEND_FUNCTION(alarm) {
	zval *ontimeout;
	zval *retval_ptr;

	//参数获取
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ontimeout) != SUCCESS) {
		RETURN_LONG(0);
		return;
	}
	

	DWORD dwThreadID = 0;
    HANDLE handleFirst = NULL;
	if (setjmp(j) == 0)
	{
		//printf("\''setjmp\'' is initializing \''j\''\n");
		//Sleep(1000000);
		//raise_exception();

		handleFirst= CreateThread(NULL, 0, ThreadFuncFirst, 0, 0, &dwThreadID);
	    if (!handleFirst)
		{
			RETURN_LONG(0);
		}
		
		//raise_exception();
		//HANDLE arrayHandle[] = {handleFirst, handleSecond};
		//WaitForMultipleObjects(2, arrayHandle, TRUE, INFINITE);
		

		//printf("this line should never appear\n");
	}
	else
	{
	//	printf("''setjmp'' was just jumped into\n");

		if (SUCCESS != call_user_function(EG(function_table), NULL, ontimeout, retval_ptr, 0, NULL)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "ontimeout callback fail");
		}

		/* this code is the exception handler */
	}
	WaitForSingleObject(handleFirst, INFINITE);//等待线程返回，用sleep()就太山寨了
	CloseHandle(handleFirst);//句柄默认值2 这里减1，线程函数执行完后释放资源。
	RETURN_LONG(1);

}



static zend_function_entry wing_process_methods[] = {
	ZEND_ME(wing_process, __construct,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_ME(wing_process, __destruct, NULL,ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	ZEND_ME(wing_process, wait,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, run,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getProcessId,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getThreadId,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getCommandLine,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, kill,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getMemory,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getCurrentProcessId,  NULL,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{
	    NULL,NULL,NULL
	}
};


PHP_MINIT_FUNCTION(wing_process)
{

	PHP_PATH = (char*)malloc(MAX_PATH);
	memset(PHP_PATH, 0, MAX_PATH);
	GetModuleFileName(NULL, PHP_PATH, MAX_PATH);

	REGISTER_STRING_CONSTANT("WING_PROCESS_PHP",     PHP_PATH,                 CONST_CS | CONST_PERSISTENT );
	REGISTER_STRING_CONSTANT("WING_PROCESS_VERSION", PHP_WING_PROCESS_VERSION, CONST_CS | CONST_PERSISTENT );

	zend_class_entry _wing_process_ce;
	//INIT_CLASS_ENTRY(_wing_process_ce, "wing_process", wing_process_methods);
	INIT_NS_CLASS_ENTRY(_wing_process_ce, "wing", "wing_process", wing_process_methods);
	wing_process_ce = zend_register_internal_class(&_wing_process_ce TSRMLS_CC);
	
	zend_declare_property_string(wing_process_ce, "file", strlen("file"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_string(wing_process_ce, "output_file", strlen("output_file"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "process_info_pointer", strlen("process_info_pointer"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	//zend_declare_property_long(wing_process_ce, "redirect_handler", strlen("redirect_handler"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_string(wing_process_ce, "command_line", strlen("command_line"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "process_id", strlen("process_id"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "thread_id", strlen("thread_id"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);


	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(wing_process)
{
	
	if( PHP_PATH )
		free(PHP_PATH);

	return SUCCESS;
}

PHP_RINIT_FUNCTION(wing_process)
{
#if defined(COMPILE_DL_WING_PROCESS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
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
	PHP_FE(alarm, NULL)
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
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(wing_process)
#endif

