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

#define WING_ERROR_FAILED  0
#define WING_ERROR_SUCCESS 1


BOOL wing_check_is_runable(char *file);

static int le_wing_process;
char *PHP_PATH = NULL;

zend_class_entry *wing_process_ce;

ZEND_METHOD(wing_process, __construct) {
	
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

	char *command_line = "";

	//if file is a process id,so here we check the file param is number ?
	if (is_numeric_string(file, strlen(file), NULL, NULL, 0)) 
	{
		//if create by a process id
		PROCESSINFO *item = new PROCESSINFO();
		DWORD process_id = zend_atoi(file, strlen(file));
		//query the process info by ntll.dll
		WingQueryProcessByProcessID(item, process_id);
		if (item)
		{
			spprintf(&command_line, 0, "%s", item->command_line);
			zend_update_property_long(wing_process_ce, getThis(), "process_id", strlen("process_id"), process_id TSRMLS_CC);
			zend_update_property_long(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), 0 TSRMLS_CC);
			delete item;
		}
	}
	else 
	{
		if (!wing_check_is_runable(file))
			spprintf(&command_line, 0, "%s %s\0", PHP_PATH, file);
		else
			spprintf(&command_line, 0, "%s\0", file);
	}

	zend_update_property_string(wing_process_ce, getThis(), "command_line", strlen("command_line"), command_line TSRMLS_CC);
}


ZEND_METHOD(wing_process, __destruct) {

	zval *_pi = zend_read_property(wing_process_ce, getThis(),
		"process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);

	if (pi) {
		CloseHandle(pi->hProcess);
		CloseHandle(pi->hThread);
		delete pi;
	}

	zval *command_line = zend_read_property(wing_process_ce, getThis(), 
		"command_line", strlen("command_line"), 0, 0 TSRMLS_CC);
	if( Z_STRVAL_P(command_line) )
		efree(Z_STRVAL_P(command_line));
		
}

BOOL wing_check_is_runable(char *file) {

	char *begin     = NULL;
	char *find      = NULL;

	if ( file[0] == '\'' || file[0] == '\"' ) {
		 begin = file + 1;
	}
	else {
		begin = file;
	}


	find = strchr(begin, '.');
	if (!find)
		return 0;
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

ZEND_METHOD(wing_process, run) {


	zval *file         = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);
	char *php_file     = Z_STRVAL_P(file);
	zval *_output_file = zend_read_property(wing_process_ce, getThis(), "output_file", strlen("output_file"), 0, 0 TSRMLS_CC);
	char *output_file  = Z_STRVAL_P(_output_file);
	

	zval *_command = zend_read_property(wing_process_ce, getThis(), "command_line", strlen("command_line"), 0, 0 TSRMLS_CC);

	char *command      = Z_STRVAL_P(_command);

	/*if( !wing_check_is_runable(php_file) )
		spprintf(&command, 0, "%s %s\0", PHP_PATH, php_file);
	else
		spprintf(&command, 0, "%s\0", php_file);*/


	HANDLE m_hRead         = NULL;
	HANDLE m_hWrite        = NULL;
	STARTUPINFO sui;
	PROCESS_INFORMATION *pi=new PROCESS_INFORMATION();                        // 保存了所创建子进程的信息
	SECURITY_ATTRIBUTES sa;                        // 父进程传递给子进程的一些信息



	sa.bInheritHandle = TRUE;                // 还记得我上面的提醒吧，这个来允许子进程继承父进程的管道句柄
	sa.lpSecurityDescriptor = NULL;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);

	/*if (!CreatePipe(&m_hRead, &m_hWrite, &sa, 0))
	{
	efree(command);
	RETURN_LONG( WING_ERROR_FAILED );
	return;
	}*/

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

	ZeroMemory(&sui, sizeof(STARTUPINFO));         // 对一个内存区清零，最好用ZeroMemory, 它的速度要快于memset

	sui.cb = sizeof(STARTUPINFO);
	sui.dwFlags = STARTF_USESTDHANDLES;
	sui.hStdInput = NULL;//m_hRead;
	sui.hStdOutput = hConsoleRedirect;//m_hWrite;
	sui.hStdError = hConsoleRedirect;//GetStdHandle(STD_ERROR_HANDLE);

									 /*if( params_len >0 ) {
									 DWORD byteWrite  = 0;
									 if( ::WriteFile( m_hWrite, params, params_len, &byteWrite, NULL ) == FALSE ) {
									 php_error_docref(NULL TSRMLS_CC, E_WARNING, "write data to process error");
									 }
									 }*/

	if (!CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &sui, pi)) {
		CloseHandle(hConsoleRedirect);
		RETURN_LONG(WING_ERROR_FAILED);
		return;
	}

	CloseHandle(hConsoleRedirect);
	//CloseHandle(pi->hProcess);  // 子进程的进程句柄
	//CloseHandle(pi->hThread);   // 子进程的线程句柄，windows中进程就是一个线程的容器，每个进程至少有一个线程在执行
	//redirect_handler
	zend_update_property_long(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), (zend_long)pi TSRMLS_CC);
	zend_update_property_string(wing_process_ce, getThis(), "command_line", strlen("command_line"), command TSRMLS_CC);
	zend_update_property_long(wing_process_ce, getThis(), "process_id", strlen("process_id"), pi->dwProcessId TSRMLS_CC);
	zend_update_property_long(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), pi->dwThreadId TSRMLS_CC);

	//efree(command);
	RETURN_LONG(pi->dwProcessId);
	//delete pi;
	return;

}


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
		return;
	}
	if (GetExitCodeProcess(process, &wait_result) == 0) {

		RETURN_LONG(WING_ERROR_FAILED);
		return;
	}

	RETURN_LONG(wait_result);
	return;
}

ZEND_METHOD(wing_process, getProcessId) {
	zval *process_id = zend_read_property(wing_process_ce, getThis(), "process_id", strlen("process_id"), 0, 0 TSRMLS_CC);

	//PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
	RETURN_LONG(Z_LVAL_P(process_id));
}

ZEND_METHOD(wing_process, getThreadId) {
	zval *thread_id = zend_read_property(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), 0, 0 TSRMLS_CC);

	//PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
	RETURN_LONG(Z_LVAL_P(thread_id));
}
ZEND_METHOD(wing_process, getCommandLine)
{
	zval *file = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);

	if (is_numeric_string(Z_STRVAL_P(file), Z_STRLEN_P(file), NULL, NULL, 0)) {
		PROCESSINFO *item = new PROCESSINFO();
		WingQueryProcessByProcessID(item, zend_atoi(Z_STRVAL_P(file), Z_STRLEN_P(file)));
		if (item)
		{
			char *command_line;
			spprintf(&command_line, 0, "%s", item->command_line);
			delete item;
			RETURN_STRING(command_line)
		}
	}
	else {
		zval *command_line = zend_read_property(wing_process_ce, getThis(),
			"command_line", strlen("command_line"), 0, 0 TSRMLS_CC);
		RETURN_STRING(Z_STRVAL_P(command_line));
	}
}
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
static zend_function_entry wing_process_methods[] = {
	ZEND_ME(wing_process, __construct,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_ME(wing_process, __destruct, NULL,ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	ZEND_ME(wing_process, wait,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, run,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getProcessId,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getThreadId,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, getCommandLine,  NULL,ZEND_ACC_PUBLIC)
	ZEND_ME(wing_process, kill,  NULL,ZEND_ACC_PUBLIC)
	{
	NULL,NULL,NULL
	}
};


PHP_MINIT_FUNCTION(wing_process)
{

	PHP_PATH = (char*)emalloc(MAX_PATH);
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
		efree(PHP_PATH);

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

