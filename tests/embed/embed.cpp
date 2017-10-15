// embed.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "sapi/embed/php_embed.h"

int main(int argc, char * argv[])
{
	PHP_EMBED_START_BLOCK(argc, argv);
	char * script = " echo 'hello world';";
	//zval res;
	//MAKE_STD_ZVAL(res);
	zend_eval_string(script, NULL/*&res*/,
		"Simple Hello World App" TSRMLS_CC);
	//char *_res = (char*)Z_STR_P(res);
	//printf("%s", Z_STR(res));
	PHP_EMBED_END_BLOCK();
    return 0;
}

