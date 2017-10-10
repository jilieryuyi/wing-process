/**
 *@�ַ�������
 *@auth yuyi
 *@email 297341015@qq.com
 */
#define _CRT_GETPUTWCHAR_NOINLINE
#include "WingString.class.h"
#include "stdio.h"

WingString::WingString(char *_str, size_t _size)
{
	
	if (_size <= 0) 
		_size = WING_CHAR_SIZE(_str);
	
	this->str      = malloc(_size);
	this->str_size = _size;
	this->str_type = WING_STR_IS_CHAR;

	memset(this->str, 0, _size);
	memcpy(this->str, _str, _size); 
}

WingString::WingString(wchar_t *_str, size_t _size)
{
	
	if (_size <= 0) 
		_size = WING_WCHAR_SIZE(_str);
	
	this->str      = malloc(_size);
	this->str_size = _size;
	this->str_type = WING_STR_IS_WCHAR;

	memset(this->str, 0x0, _size);
	memcpy(this->str, _str, _size); 
}

WingString::WingString()
{
	this->str      = NULL;
	this->str_size = 0;
	this->str_type = WING_STR_IS_UNKNOW;
}

WingString::~WingString()
{
	if (this->str != NULL) {
		free(this->str);
		this->str = NULL;
	}
	this->str_size = 0;
	this->str_type = WING_STR_IS_UNKNOW;
}


int WingString::operator != (WingString &_str) const
{
	return !(*this == _str);
}
int WingString::operator != (const char* _str) const
{
	return !(*this == _str);
}

int WingString::operator != (const wchar_t* _str) const
{
	return !(*this == _str);
}

int WingString::operator < (const wchar_t* _str) const
{
	switch (this->str_type) {
		case WING_STR_IS_UNKNOW:
			return _str!=NULL;
		break;
		case WING_STR_IS_WCHAR: {
			return wcscmp((wchar_t*)this->str, _str) < 0;
		}
		break;
		case WING_STR_IS_CHAR:{
			wchar_t *res = wing_str_char_to_wchar((char*)this->str);
			int d = wcscmp(res, _str);
			if (res) {
				free(res);
			}
			return d < 0;
		}
		break;
	}
	return false;
}

int WingString::operator <= (const wchar_t* _str) const
{
	switch (this->str_type) {
		case WING_STR_IS_UNKNOW:
			//NULL ��ԶС�ڵ���һ���ַ���
			return 1;
		break;
		case WING_STR_IS_WCHAR: {
			return wcscmp((wchar_t*)this->str, _str) <= 0;
		}
		break;
		case WING_STR_IS_CHAR: {
			wchar_t *res = wing_str_char_to_wchar((char*)this->str);
			int d = wcscmp(res, _str);
			if (res) {
				free(res);
			}
			return d <= 0;
		}break;
	}
	return false;
}

int WingString::operator < (const char* _str) const
{
	switch (this->str_type) {
		case WING_STR_IS_UNKNOW:
			return _str != NULL;
		break;
		case WING_STR_IS_CHAR: {
			return strcmp((char*)this->str, _str) < 0;
		}
		break;
		case WING_STR_IS_WCHAR:	{
			char *res = wing_str_wchar_to_char((wchar_t*)this->str);
			int d = strcmp(res, _str);
			if (res) free(res);
			return d < 0;

		}break;
	}
	return false;
}

int WingString::operator <= (const char* _str) const
{
	switch (this->str_type) {
	    case WING_STR_IS_UNKNOW:
		    return 1;
		break;
	    case WING_STR_IS_CHAR: {
		    return strcmp((char*)this->str, _str) <= 0;
	    }
	   break;
    	case WING_STR_IS_WCHAR: {
	    	char *res = wing_str_wchar_to_char((wchar_t*)this->str);
		    int d = strcmp(res, _str);
		    if (res) free(res);
		    return d <= 0;
	    }
	    break;
	}
	return false;
}

int WingString::operator < (WingString &_str) const
{
	switch (_str.type()) {
		case WING_STR_IS_UNKNOW:
			return 0;
		break;
		case WING_STR_IS_CHAR: {
			if (this->str_type == WING_STR_IS_CHAR) {
				return strcmp((char*)this->str, (char*)_str.data()) < 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return _str.type() != WING_STR_IS_UNKNOW;
			} else if (this->str_type == WING_STR_IS_WCHAR) {
				char *res = wing_str_wchar_to_char((wchar_t*)this->str);
				int d = strcmp(res, (char*)_str.data());
				if (res) free(res);
				return d < 0;
			}
			return false;
		}
		break;
		case WING_STR_IS_WCHAR: {
			if (this->str_type == WING_STR_IS_WCHAR) {
				return wcscmp((wchar_t*)this->str, (wchar_t*)_str.data()) < 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return _str.type() != WING_STR_IS_UNKNOW;
			} else if (this->str_type == WING_STR_IS_CHAR) {
				wchar_t *res = wing_str_char_to_wchar((char*)this->str);
				int d = wcscmp(res, (wchar_t*)_str.data());
				if (res) free(res);
				return d < 0;
			}
			return false;
		}
		break;
	}
	return false;
}

int WingString::operator <= (WingString &_str) const
{
	switch (_str.type()) {
		case WING_STR_IS_UNKNOW:
			return this->str_type == WING_STR_IS_UNKNOW;
		break;
		case WING_STR_IS_CHAR: {
			if (this->str_type == WING_STR_IS_CHAR) {
				return strcmp((char*)this->str, (char*)_str.data()) <= 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return 1;
			} else if (this->str_type == WING_STR_IS_WCHAR) {
				char *res = wing_str_wchar_to_char((wchar_t*)this->str);
				int d = strcmp(res, (char*)_str.data());
				if (res) free(res);
				return d <= 0;
			}
			return false;
		}
		break;
	    case WING_STR_IS_WCHAR:	{
			if (this->str_type == WING_STR_IS_WCHAR) {
			    return wcscmp((wchar_t*)this->str, (wchar_t*)_str.data()) <= 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return 1;
			} else if (this->str_type == WING_STR_IS_CHAR) {
				wchar_t *res = wing_str_char_to_wchar((char*)this->str);
				int d = wcscmp(res, (wchar_t*)_str.data());
				if (res) free(res);
				return d <= 0;
			}
			return false;
		}break;
	}
	return false;
}

int WingString::operator > (const wchar_t* _str) const
{
	switch (this->str_type) {
    	case WING_STR_IS_UNKNOW:
	    	return 0;
		break;
	    case WING_STR_IS_WCHAR: {
			return wcscmp((wchar_t*)this->str, _str) > 0;
		}
		break;
	    case WING_STR_IS_CHAR: {
			wchar_t *res = wing_str_char_to_wchar((char*)this->str);
			int d = wcscmp(res, _str);
			if (res) free(res);
			return d > 0;
		}break;
	}
	return false;
}

int WingString::operator >= (const wchar_t* _str) const
{
	switch (this->str_type) {
    	case WING_STR_IS_UNKNOW:
	    	return _str == NULL;
		break;
	    case WING_STR_IS_WCHAR: {
			return wcscmp((wchar_t*)this->str, _str) >= 0;
		}
		break;
    	case WING_STR_IS_CHAR: {
			wchar_t *res = wing_str_char_to_wchar((char*)this->str);
			int d = wcscmp(res, _str);
			if (res) free(res);
			return d >= 0;
		}
		break;
	}
	return false;
}

int WingString::operator > (const char* _str) const
{
	switch (this->str_type) {
	    case WING_STR_IS_UNKNOW:
	       return 0;
		break;
	    case WING_STR_IS_CHAR: {
			return strcmp((char*)this->str, _str) > 0;
		}
		break;
	    case WING_STR_IS_WCHAR: {
			char *res = wing_str_wchar_to_char((wchar_t*)this->str);
			int d = strcmp(res, _str);
			if (res) free(res);
			return d > 0;
		}
		break;
	}
	return false;
}

int WingString::operator >= (const char* _str) const
{
	switch (this->str_type) {
    	case WING_STR_IS_UNKNOW:
	    	return _str == NULL;
		break;
    	case WING_STR_IS_CHAR: {
			return strcmp((char*)this->str, _str) >= 0;
		}
		break;
	    case WING_STR_IS_WCHAR: {
			char *res = wing_str_wchar_to_char((wchar_t*)this->str);
			int d = strcmp(res, _str);
			if (res) free(res);
			return d >= 0;
		}
		break;
	}
	return false;
}

int WingString::operator >= (WingString &_str) const
{
	switch (_str.type()) {
	    case WING_STR_IS_UNKNOW:
		    return 1;
		break;
	    case WING_STR_IS_CHAR: {
			if (this->str_type == WING_STR_IS_CHAR) {
				return strcmp((char*)this->str, (char*)_str.data()) >= 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return 0;
			} else if (this->str_type == WING_STR_IS_WCHAR) {
				char *res = wing_str_wchar_to_char((wchar_t*)this->str);
				int d = strcmp(res, (char*)_str.data());
				if (res) free(res);
				return d >= 0;
			}
			return false;
		}
		break;
	    case WING_STR_IS_WCHAR: {
			if (this->str_type == WING_STR_IS_WCHAR) {
				return wcscmp((wchar_t*)this->str, (wchar_t*)_str.data()) >= 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return 0;
			} else if (this->str_type == WING_STR_IS_CHAR) {
				wchar_t *res = wing_str_char_to_wchar((char*)this->str);
				int d = wcscmp(res, (wchar_t*)_str.data());
				if (res) free(res);
				return d >= 0;
			}
			return false;
		}break;
	}
	return false;
}

int WingString::operator > (WingString &_str) const
{
	switch (_str.type()) {
    	case WING_STR_IS_UNKNOW:
	    	return this->str_type != WING_STR_IS_UNKNOW;
		break;
	    case WING_STR_IS_CHAR: {
			if (this->str_type == WING_STR_IS_CHAR) {
				return strcmp((char*)this->str, (char*)_str.data()) > 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return 0;
			} else if (this->str_type == WING_STR_IS_WCHAR) {
				char *res = wing_str_wchar_to_char((wchar_t*)this->str);
				int d = strcmp(res, (char*)_str.data());
				if (res) free(res);
				return d > 0;
			}
			return false;
		}
		break;
	    case WING_STR_IS_WCHAR: {
			if (this->str_type == WING_STR_IS_WCHAR) {
				return wcscmp((wchar_t*)this->str, (wchar_t*)_str.data()) > 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return 0;
			} else if (this->str_type == WING_STR_IS_CHAR) {
				wchar_t *res = wing_str_char_to_wchar((char*)this->str);
				int d = wcscmp(res, (wchar_t*)_str.data());
				if (res) free(res);
				return d > 0;
			}
			return false;
		}break;
	}
	return false;
}

int WingString::operator == (const wchar_t* _str) const
{
	switch (this->str_type) {
    	case WING_STR_IS_UNKNOW:
	    	return _str == NULL;
		break;
	    case WING_STR_IS_WCHAR: {
			return wcscmp((wchar_t*)this->str, _str) == 0;
		}
		break;
	    case WING_STR_IS_CHAR: {
			char *res = wing_str_wchar_to_char(_str);
			int d = strcmp((char*)this->str, res);
			if (res) free(res);
			return d == 0;
		}
		break;
	}
	return false;
}

int WingString::operator == (const char* _str) const
{
	switch (this->str_type) {
    	case WING_STR_IS_UNKNOW:
	    	return _str == NULL;
		break;
	    case WING_STR_IS_CHAR: {
			return strcmp((char*)this->str, _str) == 0;
		}
		break;
	    case WING_STR_IS_WCHAR: {
			wchar_t *res = wing_str_char_to_wchar(_str);
			int d = wcscmp((wchar_t*)this->str, res);
			if (res) free(res);
			return d == 0;
		}
		break;
	}
	return false;
}

int WingString::operator == (WingString &_str) const
{
	switch (_str.type()) {
    	case WING_STR_IS_UNKNOW:
	    	return this->str_type == WING_STR_IS_UNKNOW;
		break;
	    case WING_STR_IS_CHAR: {
			if (this->str_type == WING_STR_IS_CHAR) {
				return strcmp((char*)this->str, (char*)_str.data()) == 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return false;
			} else if (this->str_type == WING_STR_IS_WCHAR) {
				char *res = wing_str_wchar_to_char((wchar_t*)this->str);
				int d = strcmp((char*)_str.data(), res);
				if (res) free(res);
				return d == 0;
			}
			return false;
		}
		break;
	    case WING_STR_IS_WCHAR: {
			if (this->str_type == WING_STR_IS_WCHAR) {
				return wcscmp((wchar_t*)this->str, (wchar_t*)_str.data()) == 0;
			} else if (this->str_type == WING_STR_IS_UNKNOW) {
				return false;
			} else if (this->str_type == WING_STR_IS_CHAR) {
				wchar_t *res = wing_str_char_to_wchar((char*)this->str);
				int d = wcscmp((wchar_t*)_str.data(), res);
				if (res) free(res);
				return d == 0;
			}
			return false;
		}
		break;
	}
	return false;
}

WingString &WingString::operator = (const char* _str)
{
	 this->append(_str , WING_CHAR_SIZE(_str));
     return *this;
}

WingString &WingString::operator = (const wchar_t* _str)
{
	 this->append(_str , WING_WCHAR_SIZE(_str));
     return *this;
}

WingString &WingString::operator = (WingString &_str)
{
	 this->str		= _str.copy();
	 this->str_size = _str.size();
	 this->str_type = _str.type();
     return *this;
}

WingString &WingString::operator + (WingString &_str)
{
    switch (_str.type()) {
	    case WING_STR_IS_UNKNOW :
	    break;
	    case WING_STR_IS_CHAR:
		    this->append((const char*)_str.data(), _str.size());
	    break;
	    case WING_STR_IS_WCHAR:
		    this->append((const wchar_t*)_str.data(), _str.size());
	    break;
    }
    return *this;
}

WingString &WingString::operator + (const char* _str)
{
	this->append(_str, WING_CHAR_SIZE(_str));	  
    return *this;
}

WingString &WingString::operator + (const wchar_t* _str)
{
	this->append(_str, WING_WCHAR_SIZE(_str));	  
    return *this;
}

WingString &WingString::operator += (WingString &_str)
{
    switch (_str.type()) {
	    case WING_STR_IS_UNKNOW :
	    break;
	    case WING_STR_IS_CHAR:
		    this->append((const char*)_str.data(), _str.size());
	    break;
	    case WING_STR_IS_WCHAR:
		    this->append((const wchar_t*)_str.data(), _str.size());
	    break;
    }
    return *this;
}

WingString &WingString::operator += (const char* _str)
{
	this->append(_str, WING_CHAR_SIZE(_str));	  
    return *this;
}

WingString &WingString::operator += (const wchar_t* _str)
{
	this->append(_str, WING_WCHAR_SIZE(_str));	  
    return *this;
}

size_t WingString::size()
{
	return this->str_size;
}

unsigned int WingString::length()
{
	switch (this->str_type) {
		case WING_STR_IS_CHAR:
			return (unsigned int)( this->str_size/sizeof(char)  -1);
			break;
		case WING_STR_IS_WCHAR:
			return (unsigned int)( this->str_size/sizeof(wchar_t) -1);
			break;
		default:
			return 0;
	}
	return 0;
}

/**
 *@׷���ַ���
 */
void WingString::append(const wchar_t *_str, size_t size)
{
	if (_str == NULL) {
		return;
    }

	if (size <=0) {
	    size = WING_WCHAR_SIZE(_str);
	}

	if (this->str_type == WING_STR_IS_UNKNOW) {
		this->str_type = WING_STR_IS_WCHAR;
		this->str      = malloc(size);
		this->str_size = size;

		memcpy(this->str, _str, size);
		return;
	}

	if (this->str_type == WING_STR_IS_CHAR) {
		char *res      = wing_str_wchar_to_char((const wchar_t*)_str);
		size_t len     = WING_CHAR_SIZE(res);
		size_t new_len = this->str_size + len - 1 ;
		char *new_str  = (char*)malloc(new_len);

		memset(new_str , 0 , new_len);

		char *str_begin = new_str;
		memcpy(str_begin , this->str , this->str_size - 1);

		str_begin += (this->str_size - 1);
		memcpy(str_begin , res , len);

		free(this->str);
		free(res);

		this->str      = new_str;
		this->str_size = new_len;
		return;
	}

	if (this->str_type == WING_STR_IS_WCHAR) {
		int wl          = sizeof(wchar_t);
		size_t new_size = this->str_size + size - wl;
		
		wchar_t* res    = (wchar_t*)malloc(new_size);

		memset(res, 0x0, new_size);

        //wsprintfW
        #ifdef WIN32
		wsprintfW(res, L"%s%s", this->str, _str);
		#else
		swprintf(res, new_size, L"%s%s", this->str, _str);
		#endif
		//swprintf
	
		free(this->str);

		this->str      = res;
		this->str_size = new_size;
		return;
	}
}

/**
 *@׷���ַ���
 */
void WingString::append(const char *_str, size_t size)
{
	if (_str == NULL) {
		return;
    }

	if (size <=0) {
	    size = WING_CHAR_SIZE(_str);
	}

	//��������ʱ�� û�г�ʼ��
	if (this->str_type == WING_STR_IS_UNKNOW) {
		this->str_type = WING_STR_IS_CHAR;
		this->str      = malloc(size);
		this->str_size = size;

		memcpy(this->str, _str, size);
		return;
	}

	if (this->str_type == WING_STR_IS_CHAR) {
		size_t new_size = this->str_size - 1 + size;
		char *res = (char*)malloc(new_size);
		memset(res, 0, new_size);

		char *str_start = res;
		memcpy(str_start, this->str, this->str_size - 1);
		str_start +=  this->str_size - 1;

		memcpy(str_start, _str, size);

		free(this->str);

		this->str      = res;
		this->str_size = new_size;

		return;
	}

	if (this->str_type == WING_STR_IS_WCHAR) {
		wchar_t* buf = wing_str_char_to_wchar((const char *)_str);
		size_t new_size = WING_WCHAR_SIZE(buf) - sizeof(wchar_t) + this->str_size;

		wchar_t* buffer = (wchar_t*)malloc(new_size);
		memset(buffer, 0x0, new_size);

        //wsprintfWW
        #ifdef WIN32
		wsprintfW(buffer, L"%s%s", this->str, buf);
		#else
		swprintf(buffer, new_size, L"%ls%ls", this->str, buf);
		#endif
		free(this->str);
		free(buf);

		this->str      = buffer;
		this->str_size = new_size;
	}
}
/**
 *@׷���ַ���
 */
void WingString::append(WingString &_str)
{
		
	if (this->str_type == WING_STR_IS_UNKNOW) {
		size_t size    = _str.size();
		this->str      = malloc(size);
		this->str_size = size;
		this->str_type = _str.type();

		memset(this->str, 0 , size);
		memcpy(this->str, _str.data(), size);
		return;
	}

	else if (this->str_type == WING_STR_IS_CHAR) {

		if (_str.type() == WING_STR_IS_UNKNOW) {
		    return;
		}

		else if (_str.type() == WING_STR_IS_CHAR) {
			size_t new_size = this->str_size - 1 + _str.size();
			char *res = (char*)malloc(new_size);
			memset(res, 0, new_size);

			char *str_start = res;
			memcpy(str_start, this->str, this->str_size - 1);
			str_start +=  this->str_size - 1;

			memcpy(str_start, _str.data(), _str.size());

			free(this->str);

			this->str      = res;
			this->str_size = new_size;

			return;
		}

		else if (_str.type() == WING_STR_IS_WCHAR) {

			char *res = wing_str_wchar_to_char((const wchar_t*)_str.data());

			size_t len       = WING_CHAR_SIZE(res);
			size_t new_len   = this->str_size + len - 1 ;

			char *new_str = (char*)malloc(new_len);

			memset(new_str , 0 , new_len);

			char *str_begin = new_str;
			memcpy(str_begin , this->str , this->str_size - 1);

			str_begin += (this->str_size - 1);
			memcpy(str_begin , res , len);

			free(this->str);
			free(res);

			this->str      = new_str;
			this->str_size = new_len;
			return;

		}
		return;
	}

	else if (this->str_type == WING_STR_IS_WCHAR) {
		
		if (_str.type() == WING_STR_IS_UNKNOW)
			return;

		else if (_str.type() == WING_STR_IS_WCHAR) {
			
			int wl       = sizeof(wchar_t);
			size_t new_size = this->str_size + _str.size() - wl;
		
			wchar_t* res = (wchar_t*)malloc(new_size);

			memset(res, 0x0, new_size);

            #ifdef WIN32
			wsprintfW(res, L"%s%s", this->str, _str.data());
	        #else
	        swprintf(res, new_size, L"%s%s", this->str, _str.data());
	        #endif
			free(this->str);

			this->str      = res;
			this->str_size = new_size;
			return;
		}

		else if (_str.type() == WING_STR_IS_CHAR) {
			
			wchar_t* buf = wing_str_char_to_wchar((const char *)_str.data());
			size_t new_size = WING_WCHAR_SIZE(buf) - sizeof(wchar_t) + this->str_size;

			wchar_t* buffer = (wchar_t*)malloc(new_size);
			memset(buffer, 0x0, new_size);
            #ifdef WIN32
			wsprintfW(buffer, L"%s%s", this->str, buf);
			#else
			swprintf(buffer, new_size, L"%s%s", this->str, buf);
			#endif
			free(this->str);
			free(buf);

			this->str      = buffer;
			this->str_size = new_size;
			return;

		}
		return;
	}


}
/**
 *@ԭ������
 */
void * WingString::data() {
	return this->str;
}
/**
 *@�ַ����������� char ���� wachr_t ���� unkonw ����ֵ
 *@ WING_STR_IS_CHAR   1
 *@ WING_STR_IS_WCHAR  2
 *@ WING_STR_IS_UNKNOW 3
 */
int WingString::type() {
	return this->str_type;
}
/**
 *@��������
 */
void* WingString::copy() {
	if (str == NULL || str_size <= 0)
		return NULL;
	void* res = malloc(str_size);
	memset(res,0,str_size);
	memcpy(res,str,str_size);
	return res;
}

/**
 *@�˴�����char* ��Ҫʹ��free�ͷţ����Ҳ��ı�������?
 */
char* WingString::c_str() {

	char* res = NULL;

	switch (this->str_type) {
	
	case WING_STR_IS_CHAR:
		{
			res = (char*)this->copy();
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			res = wing_str_wchar_to_char((const wchar_t*)str);	   
		}
		break;
	}

	return res;
}
/**
 *@�˴�����wchar_t* ��Ҫʹ��free�ͷţ����Ҳ��ı�������?
 */
wchar_t* WingString::w_str() {

	wchar_t* res = NULL;

	switch (this->str_type) {
	
	case WING_STR_IS_CHAR:
		{
			res = wing_str_char_to_wchar((const char*)this->str);
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			res = (wchar_t*)this->copy(); 
		}
		break;
	}

	return res;
}
/**
 *@��ӡ�ַ���
 */
void WingString::print() {
	 setlocale(LC_ALL, "chs");
	if (this->str_type == WING_STR_IS_CHAR)
		printf("---char:size=%zu,len=%d,%s---\r\n",this->size(),this->length(),(char*)this->str);
	else if (this->str_type == WING_STR_IS_WCHAR)
		wprintf(L"---wchar_t:size=%zu,len=%ld,%s---\r\n",this->size(),this->length(),(wchar_t *)this->str);
}
/**
 *@��ȫ��ӡ�ַ��������������ݰ�ȫ
 */
void WingString::savePrint() {
	setlocale(LC_ALL, "chs");

	long i   = 0;
	long end = this->length();

	if (this->str_type == WING_STR_IS_CHAR) {
		printf("---char:size=%zu,len=%d,",this->size(),this->length());
		while(i < end) {
			char c = ((char*)this->str)[i];
			if (c == '\0') c = ' ';
			printf("%c", c);
			i++;
		}
		printf("---\r\n");
	}
	else if (this->str_type == WING_STR_IS_WCHAR) {
		
		wprintf(L"---wchar_t:size=%zu,len=%ld,",this->size(),this->length());
		while(i < end) {
			wprintf(L"%c",((wchar_t*)this->str)[i]);
			i++;
		}
		
		wprintf(L"---\r\n");
	}
}


/**
 * @�ַ���ת��Ϊutf8���룬��ı�����?
 */
int WingString::toUTF8()
{
	char *utf8_str = NULL;
	switch (this->str_type) {
	case WING_STR_IS_CHAR:
		{
			utf8_str = wing_str_char_to_utf8((const char*)str);
		}
		break;
	case WING_STR_IS_UNKNOW:
		return 1;
		break;
	case WING_STR_IS_WCHAR:
		{
			utf8_str = wing_str_wchar_to_utf8((const wchar_t*)this->str);
		}
		break;
	}

	if (utf8_str != NULL) {
		free(str);
		this->str      = utf8_str;
		this->str_size = WING_CHAR_SIZE(utf8_str);
		this->str_type = WING_STR_IS_CHAR;
		return 1;
	} else
		return 0;
}

/**
 *@ȥ�����˿ո� ����ı�����?
 */
char* WingString::trim() {
	
	if (this->str == NULL || this->str_size <= 0) 
		return NULL;
	if (this->str_type == WING_STR_IS_CHAR)
	{
		wing_str_trim((char*)this->str);
		this->str_size = strlen((char*)this->str)+1;
		return (char*)this->str;
	}

	return NULL;
}

char* WingString::ltrim() {

	if (this->str == NULL || this->str_size <= 0)
		return NULL;
	if (this->str_type == WING_STR_IS_CHAR)
	{




        	char *_et = (char*)((size_t)this->str + this->str_size - 1);
        	char *_st = (char*)this->str;

//        	while (*_et == ' ') {
//        		*_et-- = '\0';
//        	}

        	while (*_st == ' ') {
        		while (_st <= _et) { *_st = *(_st + 1); _st++; }
        		_st = (char*)this->str;
        	}

		this->str_size = strlen((char*)this->str)+1;
		return (char*)this->str;
	}
	return NULL;
}

char* WingString::rtrim() {

	if (this->str == NULL || this->str_size <= 0)
		return NULL;
	if (this->str_type == WING_STR_IS_CHAR)
	{
		char *_et = (char*)((size_t)this->str + this->str_size - 2);

        while (*_et == ' ') {
            *_et-- = '\0';
        }

		this->str_size = strlen((char*)this->str)+1;
		return (char*)this->str;
	}
	return NULL;
}

/***
 * @��ȫ�Ľ��ַ���ת��Ϊdouble����
 */
double WingString::toNumber() {
	
	//0��asciiΪ48 
	//9��asciiΪ57 
	//.��ascii��Ϊ46
	//-��ascii��Ϊ45
	
	char *numstr  = NULL;
	int need_free = 0;
	switch (this->str_type) {
		case WING_STR_IS_UNKNOW:
			return 0;
			break;
		case WING_STR_IS_CHAR:
			numstr = (char*)this->str;
			break;
		case WING_STR_IS_WCHAR:
			{
				numstr = wing_str_wchar_to_char((const wchar_t*)this->str);
				if (numstr) {
					need_free = 1;
				} else
					return 0;
			}
			break;
	}
	
	int i          = 0;
	int len        = this->length();
	//int is_minus   = 0;
	//int is_decimal = 0;

	
	double result = 0;
	int maxmi     = 0;
	int haspoint  = 0;

	//������Ϊ�˵õ������ݺ����ֳ���
	while(i < len) {
		int ascii = (int)numstr[i];
		if ((ascii < 48 || ascii > 57) && ascii != 45 && ascii != 46) break;
		if ((i+1)<len) {
			if ((ascii==45 || ascii == 46) && ((int)numstr[i+1] == 45 || (int)numstr[i+1] == 46)) break;
		}
		if ((ascii >= 48 && ascii <= 57) && !haspoint) {
			maxmi++;
			
		} 
		if (ascii == 46)
			haspoint=1;
		i++;
	}

	if (i <= 0) return 0;

	int start  = 0;
	    len    = i;
	double ten = 10;

	//����Ǹ���?
	if ((int)numstr[start] == 45) {
		start = 1;
		while(start < i) {

			if ((int)numstr[start] == 46) {
				start++; 
				continue;
			}

			int m =maxmi-1;
			result -= ((int)numstr[start]-48)*pow(ten,m);

			start++;
			maxmi--;
		}

		if (need_free) 
			free(numstr);
		return result;
	}

	

	
	start = 0;
	while(start < i) {

		if ((int)numstr[start] == 46) {
			start++; 
			continue;
		}

		int m   = maxmi-1;
		result += ((int)numstr[start]-48)*pow(ten,m);

		start++;
		maxmi--;
	}

	if (need_free) 
		free(numstr);
	
	return result;
}

/**
 *@�������ַ��� ���ı��ַ�������  
 *@����֮�� ����ֵ �������ֵ��Ϊnull ��Ҫ free ,start ��0��ʼ��Ҳ�����Ǹ�������ĩβ��ʼ��ȡ
 */
void* WingString::substr(int start,size_t length) {

	//int len = this->length();
	if (this->str_type == WING_STR_IS_UNKNOW) 
		return NULL;
	
	unsigned long sl = 0;
	if (this->str_type == WING_STR_IS_CHAR)
		sl = sizeof(char);
	else if (this->str_type == WING_STR_IS_WCHAR)
		sl = sizeof(wchar_t);

	size_t end_str   = (size_t)this->str + this->str_size - 1*sl;
	size_t start_str = NULL;

	if (start >= 0) {
		start_str =  (size_t)this->str + start*sl ;
		if (start_str >= end_str) 
			return NULL;
	} else{
		start_str = (size_t)this->str + this->str_size + (start - 1)*sl;
		if (start_str < (size_t)this->str)
			start_str = (size_t)this->str;
	}

	if ((size_t)length > (end_str-start_str)/sl) 
		length = (end_str-start_str)/sl;

	void* _subatr = malloc((length+1)*sl);
	memset(_subatr, 0 , (length+1)*sl);
	memcpy(_subatr, (const void*)start_str, length*sl);

	return _subatr;
}

//----WingString end------------------------


/**
 *@wchar_tת������Ϊutf8
 */
char* wing_str_wchar_to_utf8(const wchar_t* _str) {
#ifdef WIN32
	if (_str == NULL)
		return NULL;
	int nLen = WideCharToMultiByte(CP_UTF8, 0, _str, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return NULL;
	char*  m_cUtf8 = (char*)malloc(nLen + 1);  
	memset(m_cUtf8,0,nLen + 1);
	WideCharToMultiByte (CP_UTF8, 0, _str, -1, m_cUtf8, nLen, NULL,NULL); 
	return m_cUtf8;
#endif
    return NULL;

}

/**
 *@wchar_t ת��Ϊ char
 */
char* wing_str_wchar_to_char(const wchar_t* _str) {
#ifdef WIN32
	 if (_str == NULL)
		 return NULL;
	 int nLen = WideCharToMultiByte(CP_OEMCP,NULL, _str,-1,NULL,0,NULL,FALSE);  
	 if (nLen <= 0) return NULL;
     char * m_cDest = (char*)malloc(nLen); 
	 memset(m_cDest,0,nLen);
     WideCharToMultiByte (CP_OEMCP,NULL,_str,-1, m_cDest, nLen,NULL,FALSE);  
	 return m_cDest;
#endif
    return NULL;
}
wchar_t* wing_str_char_to_wchar(const char* _str) {
#ifdef WIN32
	size_t size     = WING_CHAR_SIZE(_str);
	size_t len      = MultiByteToWideChar(CP_ACP,0,(const char *)_str,(int)(size-1),NULL,0);

	size_t buf_size = (len+1)*sizeof(wchar_t);
	wchar_t* buf = (wchar_t*)malloc(buf_size);
	memset(buf, 0x0, buf_size);
	MultiByteToWideChar(CP_ACP,0,(const char *)_str,(int)(size-1),buf,(int)len);   

	return buf;
#endif
    return NULL;
}
char* wing_str_char_to_utf8(const char* str) {
#ifdef WIN32
	if (str == NULL)
		return NULL;

	wchar_t* unicode_str = NULL;
	int utf8_str_size    = 0;

	utf8_str_size      = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);                   //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��?
	size_t msize       = (utf8_str_size + 1) * sizeof(wchar_t);
	unicode_str        = (wchar_t*)malloc(msize);                     //ΪUnicode�ַ����ռ�
	memset(unicode_str, 0x0, (utf8_str_size + 1)*sizeof(wchar_t));
	utf8_str_size      = ::MultiByteToWideChar(CP_ACP, 0, str, -1, unicode_str, utf8_str_size);   //ת����Unicode����
	
	if (!utf8_str_size)                                                                                 //ת��ʧ��������˳�?
	{
		if (unicode_str) 
			delete[] unicode_str;
		return 0;
	}

	utf8_str_size  = WideCharToMultiByte(CP_UTF8,0,unicode_str,-1,NULL,0,NULL,NULL);                    //��ȡת����UTF8���������Ҫ���ַ��ռ䳤��?
	char *utf8_str = (char*)malloc(utf8_str_size+1);

	memset(utf8_str,0,utf8_str_size+1);

	utf8_str_size = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, (char *)utf8_str, utf8_str_size+1, NULL, NULL);  
	                                                                                                    //ת����UTF8����
	if (unicode_str)
		delete []unicode_str;

	if (!utf8_str_size)
		return 0;

	return utf8_str;
#endif
    return NULL;
}

/**
 *@ȥ���ַ������˿ո�
 */
void wing_str_trim(char* str ,size_t size) {
	if (str == NULL) 
		return;
	if (size <= 0)
		size = strlen(str);
	
	char *_et = (char*)(str + size - 1);
	char *_st = str;

	while (*_et == ' ') {
		*_et-- = '\0';
	}

	while (*_st == ' ') {
		while (_st <= _et) { *_st = *(_st + 1); _st++; }
		_st = str;
	}
}
