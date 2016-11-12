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
PHP_FUNCTION(confirm_wing_process_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "wing_process", arg);

	RETURN_STR(strg);
}



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

	zend_update_property_string( wing_process_ce, getThis(), "file",        strlen("file"),        file        TSRMLS_CC );
	zend_update_property_string( wing_process_ce, getThis(), "output_file", strlen("output_file"), output_file TSRMLS_CC );

}

/**
* @ 析构函数
*/
ZEND_METHOD(wing_process, __destruct) {


}

/**
* @ 获取某个key
*/
ZEND_METHOD(wing_process, test) {

	
	zval *file = zend_read_property(wing_process_ce, getThis(), "file", strlen("file"),0, 0 TSRMLS_CC);
	char *str = Z_STRVAL_P(file);
	printf(str);
	RETURN_STRING("test function");
}


static zend_function_entry wing_process_methods[] = {
	ZEND_ME(wing_process, __construct,NULL,ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_ME(wing_process, __destruct, NULL,ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	ZEND_ME(wing_process, test,  NULL,ZEND_ACC_PUBLIC)
{
	NULL,NULL,NULL
}
};

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_wing_process_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_wing_process_init_globals(zend_wing_process_globals *wing_process_globals)
{
	wing_process_globals->global_value = 0;
	wing_process_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(wing_process)
{

	PHP_PATH = (char*)emalloc(MAX_PATH);
	memset(PHP_PATH, 0, MAX_PATH);
	GetModuleFileName(NULL, PHP_PATH, MAX_PATH);

	zend_register_string_constant("WING_PROCESS_PHP",     sizeof("WING_PROCESS_PHP"),     PHP_PATH,                 CONST_CS | CONST_PERSISTENT, module_number TSRMLS_CC);
	zend_register_string_constant("WING_PROCESS_VERSION", sizeof("WING_PROCESS_VERSION"), PHP_WING_PROCESS_VERSION, CONST_CS | CONST_PERSISTENT, module_number TSRMLS_CC);


	zend_class_entry _wing_process_ce;
	INIT_CLASS_ENTRY(_wing_process_ce, "wing_process", wing_process_methods);
	wing_process_ce = zend_register_internal_class(&_wing_process_ce TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "file", strlen("file"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(wing_process_ce, "output_file", strlen("output_file"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);


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
	PHP_FE(confirm_wing_process_compiled,	NULL)		/* For testing, remove later. */
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
