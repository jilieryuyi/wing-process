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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_wing_process.h"

#include "Shlwapi.h"
#pragma comment(lib,"Shlwapi.lib")

#define WING_ERROR_FAILED  0
#define WING_ERROR_SUCCESS 1

/* If you declare any globals in php_wing_process.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(wing_process)
*/

/* True global resources - no need for thread safety here */
static int le_wing_process;
char *PHP_PATH = NULL;
/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("wing_process.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_wing_process_globals, wing_process_globals)
    STD_PHP_INI_ENTRY("wing_process.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_wing_process_globals, wing_process_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_wing_process_compiled(string arg)
   Return a string to confirm that the module is compiled in */



zend_class_entry *wing_process_ce;

/**
* @ 构造函数
*/
ZEND_METHOD(wing_process, __construct) {
	
	char *file        = NULL;  
	char *output_file = NULL;
	int file_len      = 0;
	int output_len    = 0;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &file, &file_len, &output_file, &output_len)) {
		return;
	}

	if (!PathFileExists(file)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "file <%s> does not found", file);
		return;
	}

	zend_update_property_string( wing_process_ce, getThis(), "file",        strlen("file"),        file        TSRMLS_CC );
	zend_update_property_string( wing_process_ce, getThis(), "output_file", strlen("output_file"), output_file TSRMLS_CC );

}

/**
* @ 析构函数
*/
ZEND_METHOD(wing_process, __destruct) {
	
	zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
	CloseHandle(pi->hProcess); 
	CloseHandle(pi->hThread); 

	zval *command_line = zend_read_property(wing_process_ce, getThis(), 
		"command_line", strlen("command_line"), 0, 0 TSRMLS_CC);
	efree(Z_STRVAL_P(command_line));
	

	delete pi;
}

ZEND_METHOD(wing_process, run) {


	zval *file         = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"), 0, 0 TSRMLS_CC);
	char *php_file     = Z_STRVAL_P(file);
	zval *_output_file = zend_read_property(wing_process_ce, getThis(), "output_file", strlen("output_file"), 0, 0 TSRMLS_CC);
	char *output_file  = Z_STRVAL_P(_output_file);
	
	char *command      = NULL;

	


	spprintf(&command, 0, "%s %s\0", PHP_PATH, php_file);

	//HANDLE m_hRead         = NULL;
	//HANDLE m_hWrite        = NULL;
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
		// CloseHandle(m_hWrite);
		efree(command);
		RETURN_LONG(WING_ERROR_FAILED);
		return;
	}

	CloseHandle(hConsoleRedirect);
	//CloseHandle(pi->hProcess);  // 子进程的进程句柄
	//CloseHandle(pi->hThread);   // 子进程的线程句柄，windows中进程就是一个线程的容器，每个进程至少有一个线程在执行
	//redirect_handler
	zend_update_property_long(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), (zend_long)pi TSRMLS_CC);
	zend_update_property_string(wing_process_ce, getThis(), "command_line", strlen("command_line"), command TSRMLS_CC);

	//efree(command);
	RETURN_LONG(pi->dwProcessId);
	//delete pi;
	return;

}

/**
*@wait process进程等待
*@param process id 进程id
*@param timeout 等待超时时间 单位毫秒
*@return exit code 进程退出码
*/
ZEND_METHOD(wing_process, wait) {
	int process_id, timeout = INFINITE;

	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &timeout);

	zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);


	DWORD wait_result = 0;
	DWORD wait_status = WaitForSingleObject(pi->hProcess, timeout);

	if (wait_status != WAIT_OBJECT_0) {
		RETURN_LONG(wait_status);
		return;
	}
	if (GetExitCodeProcess(pi->hProcess, &wait_result) == 0) {

		RETURN_LONG(WING_ERROR_FAILED);
		return;
	}

	RETURN_LONG(wait_result);
	return;
}

ZEND_METHOD(wing_process, getProcessId) {
	zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
	RETURN_LONG(pi->dwProcessId);
}

ZEND_METHOD(wing_process, getThreadId) {
	zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
	RETURN_LONG(pi->dwThreadId);
}
ZEND_METHOD(wing_process, getCommandLine)
{
	zval *command_line = zend_read_property(wing_process_ce, getThis(),
		"command_line", strlen("command_line"), 0, 0 TSRMLS_CC);
	RETURN_STRING(Z_STRVAL_P(command_line));
}
ZEND_METHOD(wing_process, kill)
{
	zval *_pi = zend_read_property(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), 0, 0 TSRMLS_CC);

	PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)Z_LVAL_P(_pi);
	if (!TerminateProcess(pi->hProcess, 0)) {

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
	INIT_CLASS_ENTRY(_wing_process_ce, "wing_process", wing_process_methods);
	wing_process_ce = zend_register_internal_class(&_wing_process_ce TSRMLS_CC);
	
	zend_declare_property_string(wing_process_ce, "file", strlen("file"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_string(wing_process_ce, "output_file", strlen("output_file"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "process_info_pointer", strlen("process_info_pointer"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	//zend_declare_property_long(wing_process_ce, "redirect_handler", strlen("redirect_handler"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_string(wing_process_ce, "command_line", strlen("command_line"), "", ZEND_ACC_PRIVATE TSRMLS_CC);


	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(wing_process)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/

	if( PHP_PATH )
		efree(PHP_PATH);

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(wing_process)
{
#if defined(COMPILE_DL_WING_PROCESS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(wing_process)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(wing_process)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "wing_process support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ wing_process_functions[]
 *
 * Every user visible function must have an entry in wing_process_functions[].
 */
const zend_function_entry wing_process_functions[] = {
	//PHP_FE(wing_process_exit,NULL)
	PHP_FE_END	/* Must be the last line in wing_process_functions[] */
};
/* }}} */

/* {{{ wing_process_module_entry
 */
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
/* }}} */

#ifdef COMPILE_DL_WING_PROCESS
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(wing_process)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
