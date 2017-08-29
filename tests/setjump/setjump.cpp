// setjump.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
/*jmp_buf j;
void raise_exception(void)
{
	printf("exception raised\n");
	longjmp(j, 1);
	printf("this line should never appear\n");
}
*/
void test111(char* buffer) {
	strcpy(buffer, "hello");
}

void test222(char** buffer) {
	strcpy(*buffer, "hello");
}

int main()
{
	char a[32] = {0};
	//char *a = (char*)malloc(32);
	//memset(a,0, 32);
	test111(a);
	//strcpy(a, "hello");
	printf("%s", a);


	char *b= (char*)malloc(32);
	memset(b,0, 32);
	test222(&b);
	//strcpy(a, "hello");
	printf("%s", b);
	free(b);

	/*if (setjmp(j) == 0)
	{
		printf("\''setjmp\'' is initializing \''j\''\n");
		raise_exception();
		printf("this line should never appear\n");
	}
	else
	{
		printf("''setjmp'' was just jumped into\n");
	}*/
	return 0;
}