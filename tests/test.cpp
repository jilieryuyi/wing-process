// test.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include "Windows.h"

void str_replace( char **str) {
	
	while ( (*str) != '\0') {
		if ((**str) == '\\')
			(**str) = '/';
		(*str)++;
	}
}

int main()
{

	char *str = "D:\xampp\bin\php.exe";
	char *t = _strdup(str);
	 str_replace(&t);

	 printf(t);
	 free(t);
	


	return 0;

	char *file = "D:/Wing/Release/Wing.exe";

	char first_char = file[0];
	char *begin = NULL;
	char *find = NULL;
	if (file[0] == '\'' || file[0] == '\"') {

		begin = file + 1;
		find = strchr(begin, file[0]);

	}
	else {
		begin = file;
		find = strchr(begin, ' ');
		
	}

	if (!find) {
		find = strchr(begin, '.');
		const char *p = strchr(begin, '.') + 1;

		char *ext = (char*)malloc(4);
		memset(ext, 0, 4);
		strncpy_s(ext, 4, p, 3);

		if (strcmp(ext, "exe") == 0 || strcmp(ext, "bat") == 0)
		{
			printf("is run\r\n");
		}
		printf("ext=%s\r\n", ext);
		return 0;
	}

	char *exe = (char*)malloc(find - begin + 1);
	memset(exe, 0, find - begin + 1);
	strncpy_s(exe, (find - begin + 1), begin, find - begin);
	printf("exe=%s\r\n", exe);

	const char *p = strchr(exe, '.') + 1;

	char *ext = (char*)malloc(4);
	memset(ext, 0, 4);
	strncpy_s(ext, 4, p, 3);
	printf("ext=%s\r\n", ext);

	if (strcmp(ext , "exe") == 0 || strcmp(ext,   "bat")==0)
	{
		printf("is run\r\n");
	}

	free(exe);
	free(ext);

	return 0;
}

