// setjump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <setjmp.h>
#include <stdio.h>
jmp_buf j;
void raise_exception(void)
{
	printf("exception raised\n");
	longjmp(j, 1); /* jump to exception handler */
	printf("this line should never appear\n");
}
int main(void)
{
	if (setjmp(j) == 0)
	{
		printf("\''setjmp\'' is initializing \''j\''\n");
		raise_exception();
		printf("this line should never appear\n");
	}
	else
	{
		printf("''setjmp'' was just jumped into\n");
		/* this code is the exception handler */
	}
	return 0;
}
/* When run yields:
''setjmp'' is initializing ''j''
exception raised
''setjmp'' was just jumped into
*/
//那个填充jmp_buf的函数不在调用longjmp()之前返回。否则，存储在jmp_buf中的上下文就有问题了：
/*jmp_buf j;
void f(void)
{
	setjmp(j);
}
int main(void)
{
	f();
	longjmp(j, 1); 
	return 0;
}
*/
