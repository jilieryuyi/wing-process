#ifndef __WING_STRING_H__
#define __WING_STRING_H__
#include "Windows.h"
#include <locale.h>
#include "math.h"

#define WING_STR_IS_CHAR   1
#define WING_STR_IS_WCHAR  2
#define WING_STR_IS_UNKNOW 3

#define WING_CHAR_SIZE(str)  (strlen((char*)str)+1)*sizeof(char)
#define WING_WCHAR_SIZE(str) (wcslen((wchar_t*)str)+1)*sizeof(wchar_t)

/**
 *@size为字符串长度,或者sizeof 都可以 ，已经兼容并且二进制安全
 */
void      wing_str_trim( _Inout_ char* str ,int size = 0 );
char*     wing_str_wchar_to_char( _In_ const wchar_t* str );
wchar_t*  wing_str_char_to_wchar( _In_ const char* str );
char*     wing_str_char_to_utf8( _In_ const char* str );
char*     wing_str_wchar_to_utf8( _In_ const wchar_t* str );

/**
 *@---- WingString ----
 *@字符串处理封装 
 *@注意 size 均为占用内存字节 并非字符串长度，如 size=sizeof(*data) 或者 size = (strlen(str)+1)*sizeof(char)
 */
class WingString{

private:
	void *str;
	unsigned int str_size;
	unsigned int str_type;

public:

	//构造函数
	WingString( char *_str, int _size = 0 );
	WingString( wchar_t *_str, int _size = 0 );
	WingString( );
	~WingString( );

	unsigned int size();
	unsigned int length();

	//拷贝字符串原型数据 用完需要free
	void* copy();
	//返回字符串原型数据 不改变自身 无需free
	void* data();
	//返回字符串类型
	int   type();

	//返回char*字符串 不改变自身 返回值用完需要free
	char* c_str();
	//返回wchar_t*字符串 不改变自身 返回值用完需要free
	wchar_t* w_str();

	//追加字符串 改变字符串本身
	void append( const char *_str, int size = 0 );
	void append( WingString &_str );
	void append( const wchar_t *_str,int size = 0 );

	//转换编码 改变字符串本身
	BOOL toUTF8( );
	//去掉两端空格 改变字符串本身
	void trim();

	//打印函数 一般用于调试
	void print();
	void savePrint();
	
	//转换为数字 不改变字符串本身
	double toNumber();

	//返回子字符串 不改变字符串本身  用完之后 返回值 需要 free ,start 从0开始，也可以是负数，从末尾开始截取
	void* substr(int start,int length);


	WingString& operator=(WingString &_str );
	WingString& operator=(const char* _str );
	WingString& operator=(const wchar_t* _str );
	WingString& operator+(WingString &_str );
	WingString& operator+(const char* _str );
	WingString& operator+(const wchar_t* _str );
	WingString& operator+=(WingString &_str );
	WingString& operator+=(const char* _str );
	WingString& operator+=(const wchar_t* _str );
	BOOL operator==( WingString &_str )const;
	BOOL operator==( const char* _str )const;
	BOOL operator==( const wchar_t* _str )const;
	BOOL operator!=( WingString &_str )const;
	BOOL operator!=( const char* _str )const;
	BOOL operator!=( const wchar_t* _str )const;
	BOOL operator>( WingString &_str )const;
	BOOL operator>=( WingString &_str )const;
	BOOL operator>( const char* _str )const;
	BOOL operator>=( const char* _str )const;
	BOOL operator>( const wchar_t* _str )const;
	BOOL operator>=( const wchar_t* _str )const;
	BOOL operator<( WingString &_str )const;
	BOOL operator<=( WingString &_str )const;
	BOOL operator<( const char* _str )const;
	BOOL operator<=( const char* _str )const;
	BOOL operator<( const wchar_t* _str )const;
	BOOL operator<=( const wchar_t* _str )const;

};


#endif