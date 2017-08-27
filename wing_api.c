#include "wing_api.h"

/**
 * 判断是否为php文件，此方法依据php文件开头的 <?php 识别
 *
 * @param char* file
 * @return BOOL
 */
int wing_file_is_php(const char *file)
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


ZEND_API zval *wing_zend_read_property(zend_class_entry *scope, zval *object, const char *name)
{
    TSRMLS_FETCH();
    //#if PHP_MAJOR_VERSION >= 7
	return zend_read_property(scope, object, name, strlen(name), 0 TSRMLS_CC);
//	#else
//	return zend_read_property(scope, object, name, strlen(name), 0 TSRMLS_CC);
//	#endif
}