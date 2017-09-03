// wing_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Locale.h>
#include "../../win32/WingString.class.h"
#include "../../win32/WingString.class.cpp"
#define WIN32 1

class Test {
private:
	char str[1024];
public:
	Test() {
		memset(this->str,0,1024);
		strcpy(this->str, "hello word");
	}

	void print() {
		printf("%s\r\n", this->str);
	}
};
int main()
{
	char str[] = " 123   ";
	wing_str_trim(str);
	printf(">%s<", str);

	Test test;
	test.print();

	WingString wstr1("1");
	WingString wstr2("2");

	if (wstr1 < L"2") {
		printf("bu deng\r\n");
	}
	else {
		printf("deng\r\n");
	}

	//WingString 的测试用例
	setlocale(LC_ALL, "chs");
	char trim_str1[] = " 123 ";
	wing_str_trim(trim_str1);
	printf("==>%s<==\r\n", trim_str1);

	char* wing_str1 =  wing_str_wchar_to_char(L"你好中国");
	printf("1==>%s<==\r\n", wing_str1);
	if (wing_str1) free(wing_str1);
	
	wchar_t* wing_str2 = wing_str_char_to_wchar("你好中国");
	wprintf(L"2==>%ls<==\r\n" ,wing_str2);
	if (wing_str2) free(wing_str2);

	char* wing_str3 = wing_str_char_to_utf8("你好中国");
	printf("3==>%s<==\r\n", wing_str3);
	if (wing_str3) free(wing_str3);

	char* wing_str4 = wing_str_wchar_to_utf8(L"你好中国");
	printf("4==>%s<==\r\n", wing_str4);
	if (wing_str4) free(wing_str4);

	WingString str1;
	WingString str2("123中国");
	WingString str3(L"456中国");

	printf("str1 size:%zu,length:%d\r\n", str1.size(), str1.length());
	printf("str2 size:%zu,length:%d\r\n", str2.size(), str2.length());
	printf("str3 size:%zu,length:%d\r\n", str3.size(), str3.length());

	WingString str4("123中国", sizeof("123中国"));
	WingString str5(L"456中国", sizeof(L"123中国"));

	//printf("str1 size:%zu,length:%d\r\n", str1.size(), str1.length());
	printf("str4 size:%zu,length:%d\r\n", str4.size(), str4.length());
	printf("str5 size:%zu,length:%d\r\n", str5.size(), str5.length());

    return 0;
}

