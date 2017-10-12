#ifndef __WING_STRING_H__
#define __WING_STRING_H__
#ifdef WIN32
#include "Windows.h"
#endif
#include <locale.h>
#include "math.h"

#define WING_STR_IS_CHAR   1
#define WING_STR_IS_WCHAR  2
#define WING_STR_IS_UNKNOW 3

#define WING_CHAR_SIZE(str)  (strlen((char*)str)+1)*sizeof(char)
#define WING_WCHAR_SIZE(str) (wcslen((wchar_t*)str)+1)*sizeof(wchar_t)

void      wing_str_trim(char* str ,size_t size = 0);
/**
 * need free the return value if not null
 */
char*     wing_str_wchar_to_char(const wchar_t* str);
/**
 * need free the return value if not null
 */
wchar_t*  wing_str_char_to_wchar(const char* str);
/**
 * need free the return value if not null
 */
char*     wing_str_char_to_utf8(const char* str);
/**
 * need free the return value if not null
 */
char*     wing_str_wchar_to_utf8(const wchar_t* str);

/**
 * ---- WingString ----
 */
class WingString{

private:
	void *str;
	size_t str_size;
	unsigned int str_type;

public:

	WingString(char *_str, size_t _size = 0);
	WingString(wchar_t *_str, size_t _size = 0);
	WingString();
	~WingString();

	size_t size();
	unsigned int length();

	/**
	* need free the return value if not null
	*/
	void* copy();

	void* data();
	int   type();

	/**
	* need free the return value if not null
	*/
	char* c_str();
	/**
	* need free the return value if not null
	*/
	wchar_t* w_str();

	void append(const char *_str, size_t size = 0);
	void append(WingString &_str);
	void append(const wchar_t *_str,size_t size = 0);

	int toUTF8();
	char* trim();
	char* ltrim();
	char* rtrim();

	void print();
	void savePrint();
	double toNumber();
	void* substr(int start,size_t length);

	WingString& operator=(WingString &_str);
	WingString& operator=(const char* _str);
	WingString& operator=(const wchar_t* _str);
	WingString& operator+(WingString &_str);
	WingString& operator+(const char* _str);
	WingString& operator+(const wchar_t* _str);
	WingString& operator+=(WingString &_str);
	WingString& operator+=(const char* _str);
	WingString& operator+=(const wchar_t* _str);
	int operator==(WingString &_str)const;
	int operator==(const char* _str)const;
	int operator==(const wchar_t* _str)const;
	int operator!=(WingString &_str)const;
	int operator!=(const char* _str)const;
	int operator!=(const wchar_t* _str)const;
	int operator>(WingString &_str)const;
	int operator>=(WingString &_str)const;
	int operator>(const char* _str)const;
	int operator>=(const char* _str)const;
	int operator>(const wchar_t* _str)const;
	int operator>=(const wchar_t* _str)const;
	int operator<(WingString &_str)const;
	int operator<=(WingString &_str)const;
	int operator<(const char* _str)const;
	int operator<=(const char* _str)const;
	int operator<(const wchar_t* _str)const;
	int operator<=(const wchar_t* _str)const;

};

#endif
