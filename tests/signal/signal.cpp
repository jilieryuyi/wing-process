// signal.cpp : 定义控制台应用程序的入口点。
// https://stackoverflow.com/questions/813086/can-i-send-a-ctrl-c-sigint-to-an-application-on-windows

#include "stdafx.h"
#include "windows.h"
#include "signal.h"

void SignalHandler(int signal)
{
	printf("thread id = %d, signal = %d\r\n", GetCurrentThreadId(), signal);
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
    //CTRL_C_EVENT
	printf("thread id = %d, event = %d\r\n", GetCurrentThreadId(), fdwCtrlType);
	printf("==>%d<==\r\n", fdwCtrlType);
	return false;
}

int main()
{
	signal(SIGABRT, SignalHandler);  // 22 调用abort导致， 同一个线程内部执行
	signal(SIGINT, SignalHandler);   // 2 Ctrl+C中断,回调函数在新的线程中执行
	signal(SIGTERM, SignalHandler);  // 15 kill发出的软件终止 同一个线程
	signal(SIGBREAK, SignalHandler); // 21 Ctrl+Break中断 新的线程中执行
	signal(SIGILL, SignalHandler);   // 4 非法指令 同一个线程内执行回调函数
	signal(SIGFPE, SignalHandler);   // 8 浮点异常 同一个线程内执行回调函数
	signal(SIGSEGV, SignalHandler);  // 11 段错误, 非法指针访问 同一个线程内执行回调函数

    //Ctrl+C 等事件均在新的线程中执行
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE); // add to list

	printf("thread id = %d\r\n", GetCurrentThreadId());
	//abort();
	//raise(8);
	//raise(11);

	while (1){
		Sleep(100);
	}
    return 0;
}

