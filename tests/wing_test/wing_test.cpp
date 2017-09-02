// wing_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../../win32/WingString.class.h"
#include "../../win32/WingString.class.c"


int main()
{
	char str[] = " 123   ";
	wing_str_trim(str);
	printf(">%s<", str);
    return 0;
}

