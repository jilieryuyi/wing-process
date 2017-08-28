
zval *wing_zend_read_property(zend_class_entry *scope, zval *object, const char *name)
{
	TSRMLS_FETCH();
#if PHP_MAJOR_VERSION >= 7
	return zend_read_property(scope, object, name, strlen(name), 0, 0 TSRMLS_CC);
#else
	return zend_read_property(scope, object, name, strlen(name), 0 TSRMLS_CC);
#endif
}


/**
* ?§Ø?????php??????????????php???????? <?php ???
*
* @param char* file
* @return BOOL
*/
int wing_file_is_php(const char *file)
{

	char find_str[] = " ";
	char *find = strstr((char*)file, find_str);
	char path[MAX_PATH] = { 0 };

	strncpy((char*)path, file, (size_t)(find - file));
	FILE *handle = fopen(path, "r");
	if (!handle) {
		return 0;
	}
	//char *find = NULL;
	char line[8] = { 0 };
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

