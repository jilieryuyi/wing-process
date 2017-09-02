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

void      wing_str_trim(char* str ,size_t size = 0 );
char*     wing_str_wchar_to_char( const wchar_t* str );
wchar_t*  wing_str_char_to_wchar( const char* str );
char*     wing_str_char_to_utf8( const char* str );
char*     wing_str_wchar_to_utf8( const wchar_t* str );


/**
 *---- WString ----
 */
class WString{

private:
	void *str;
	size_t str_size;
	unsigned int str_type;
	int dup;

public:

	WString(char *_str, size_t _size = 0, int dup = 0);
	WString(wchar_t *_str, size_t _size = 0, int dup = 0);
	WString();
	~WString();

    void *getStr();
	size_t getSize();
    int getType();
	size_t length();
	char* trim();
    char* ltrim();
    char* rtrim();

	void append(const char *_str, size_t size = 0);
	void append(WString &_str);
//	void append(const wchar_t *_str,size_t size = 0);

	int toUTF8();



	void print();
	void savePrint();
	
	double toNumber();

	void* substr(int start,size_t length);
};
#endif
