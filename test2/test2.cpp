// test2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"

int main()
{
	HINSTANCE  h = LoadLibrary(L"D:/php/php7-sdk/phpdev/vc14/x86/php-7.0.12/ext/wing_process/Release/wing_process.dll");
	if (h == NULL) {
		printf("加载失败");
	}
	else {
		printf("加载成功");
	}
	return 0;
}

