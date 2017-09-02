#include "WString.h"
#include "stdio.h"

WString::WString(char *_str, size_t _size, int _dup)
{
	if (_size <= 0) {
	    _size = WING_CHAR_SIZE(_str);
	} else {
	    if (_size == strlen(_str)) {
	        _size += sizeof(char);
	    }
	}

	this->str_size = _size;
	this->str_type = WING_STR_IS_CHAR;
	this->dup      = _dup;

    if (this->dup) {
        this->str = malloc(_size);
	    memset(this->str, 0, _size);
    	memcpy(this->str, _str, _size);
    } else {
        this->str      = _str;
    }
    	std::cout << "构造函数char："<<(char*)this->str<<std::endl;

}

WString::WString(wchar_t *_str, size_t _size, int _dup)
{
	if (_size <= 0) {
	    _size = WING_WCHAR_SIZE(_str);
	} else {
	    if (_size == wcslen(_str)) {
	        _size = (wcslen(_str)+1)*sizeof(wchar_t);
	    }
	}
	
	this->str_size = _size;
	this->dup      = _dup;
	this->str_type = WING_STR_IS_WCHAR;

    if (this->dup) {
	    this->str      = malloc(_size);
	    memset(this->str, 0x0, _size);
	    memcpy(this->str, _str, _size);
	} else {
	    this->str = _str;
	}
	std::cout<<"123构造函数wchar_t：";
	std::wcout <<(wchar_t*)this->str<<"=========="<<std::endl;
}

WString::WString()
{
	this->str      = NULL;
	this->str_size = 0;
	this->str_type = WING_STR_IS_UNKNOW;
}

WString::~WString()
{
	if (this->str != NULL && this->dup) {
		free(this->str);
		this->str = NULL;
	}
	this->str_size = 0;
	this->str_type = WING_STR_IS_UNKNOW;
}

size_t WString::length() {
	switch (this->str_type) {
		case WING_STR_IS_CHAR:
			return (unsigned int)(this->str_size/sizeof(char) - 1);
			break;
		case WING_STR_IS_WCHAR:
			return (unsigned int)(this->str_size/sizeof(wchar_t) - 1);
			break;
		default:
			return 0;
	}
	return 0;
}

void* WString::getStr()
{
    return this->str;
}

size_t WString::getSize()
{
    return this->str_size;
}

int WString::getType()
{
    return this->str_type;
}

/*
void WString::append(const wchar_t *_str, size_t size) {
		
	if (_str == NULL)
    {
		return;
    }

	if (size <=0) 
		size = WING_WCHAR_SIZE(_str);

	if (this->str_type == WING_STR_IS_UNKNOW)
	{
		this->str_type = WING_STR_IS_WCHAR;
		this->str      = malloc(size);
		this->str_size = size;

		memcpy(this->str, _str, size);
		return;
	}

	if (this->str_type == WING_STR_IS_CHAR) {
		
		char *res = wing_str_wchar_to_char((const wchar_t*)_str);

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

	if (this->str_type == WING_STR_IS_WCHAR) {
	    
		int wl       = sizeof(wchar_t);
		size_t new_size = this->str_size + size - wl;
		
		wchar_t* res = (wchar_t*)malloc(new_size);

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
*/
void WString::append(const char *_str, size_t size)
{
	if (_str == NULL) {
		return;
    }

	if (size <=0) {
	    size = WING_CHAR_SIZE(_str);
	}

	if (this->str_type == WING_STR_IS_UNKNOW) {
		this->str_type = WING_STR_IS_CHAR;
		this->str      = malloc(size);
		this->dup      = 1;
		this->str_size = size;

		memset(this->str, 0, size);
		memcpy(this->str, _str, size);

		return;
	}

	if (this->str_type == WING_STR_IS_CHAR) {
		size_t sub      = sizeof(char);
		size_t new_size = this->str_size - sub + size;
		char *res = (char*)malloc(new_size);
		memset(res, 0, new_size);

		char *str_start = res;
		memcpy(str_start, this->str, this->str_size - sub);
		str_start +=  this->str_size - sub;

		memcpy(str_start, _str, size);

		free(this->str);

		this->str      = res;
		this->str_size = new_size;
        this->dup      = 1;
		return;
	}

	if (this->str_type == WING_STR_IS_WCHAR) {

        printf("wchar_t appened\r\n");
		wchar_t* buf    = wing_str_char_to_wchar((const char *)_str);
		if (!buf) {
		    printf("convert fail\r\n");
		    return;
		}
		std::wcout << "===>" <<buf << std::endl;
		size_t new_size = WING_WCHAR_SIZE(buf) -sizeof(wchar_t) + this->str_size;
std::wcout << "size===>" <<new_size << std::endl;
		wchar_t* buffer = (wchar_t*)malloc(new_size);
		memset(buffer, 0x0, new_size);

        //wsprintfWW
        #ifdef WIN32
		wsprintfW(buffer, L"%s%s", this->str, buf);
		#else
		swprintf(buffer, new_size, L"%ls%ls", this->str, buf);
		#endif
//std::wcout <<(wchar_t*)this->str<<std::endl;
//		memcpy((void*)buffer,this->str,new_size);
//		std::cout << "----===>----\r\n";
//		std::wcout <<buffer << std::endl;
//
//		void *next = (void*)(buffer+this->str_size - 2*sizeof(wchar_t));
//		memcpy(next, buf, new_size - this->str_size - 2*sizeof(wchar_t));
//		std::cout << "next-\r\n";
//		std::wcout <<(wchar_t*)next << std::endl;
        std::wcout <<buffer << std::endl;

		if (this->dup && this->str) {
		    free(this->str);
		}
		free(buf);

		this->str      = buffer;
		this->str_size = new_size;
		return;
	}

}

void WString::append(WString &_str)
{
	if (this->str_type == WING_STR_IS_UNKNOW) {

		size_t size    = _str.getSize();
		this->str      = malloc(size);
		this->str_size = size;
		this->str_type = _str.getType();
		this->dup      = 1;

		memset(this->str, 0 , size);
		memcpy(this->str, _str.getStr(), size);
		return;
	}

	else if (this->str_type == WING_STR_IS_CHAR) {
        int type = _str.getType();
        size_t sub = sizeof(char);
		if (type == WING_STR_IS_UNKNOW) {
		    return;
		} else if (type == WING_STR_IS_CHAR) {
			size_t new_size = this->str_size - sub + _str.getSize();
			char *res = (char*)malloc(new_size);
			memset(res, 0, new_size);

			char *str_start = res;
			memcpy(str_start, this->str, this->str_size - sub);
			str_start +=  this->str_size - sub;

			memcpy(str_start, _str.getStr(), _str.getSize());

			free(this->str);

			this->str      = res;
			this->str_size = new_size;
			this->dup      = 1;

			return;
		}
//		else if (_str.type() == WING_STR_IS_WCHAR) {
//
//			char *res = wing_str_wchar_to_char((const wchar_t*)_str.data());
//
//			size_t len       = WING_CHAR_SIZE(res);
//			size_t new_len   = this->str_size + len - 1 ;
//
//			char *new_str = (char*)malloc(new_len);
//
//			memset(new_str , 0 , new_len);
//
//			char *str_begin = new_str;
//			memcpy(str_begin , this->str , this->str_size - 1);
//
//			str_begin += (this->str_size - 1);
//			memcpy(str_begin , res , len);
//
//			free(this->str);
//			free(res);
//
//			this->str      = new_str;
//			this->str_size = new_len;
//			return;
//
//		}
		return;
	}

//	else if (this->str_type == WING_STR_IS_WCHAR) {
//
//		if (_str.type() == WING_STR_IS_UNKNOW)
//			return;
//
//		else if (_str.type() == WING_STR_IS_WCHAR) {
//
//			int wl       = sizeof(wchar_t);
//			size_t new_size = this->str_size + _str.size() - wl;
//
//			wchar_t* res = (wchar_t*)malloc(new_size);
//
//			memset(res, 0x0, new_size);
//
//            #ifdef WIN32
//			wsprintfW(res, L"%s%s", this->str, _str.data());
//	        #else
//	        swprintf(res, new_size, L"%s%s", this->str, _str.data());
//	        #endif
//			free(this->str);
//
//			this->str      = res;
//			this->str_size = new_size;
//			return;
//		}
//
//		else if (_str.type() == WING_STR_IS_CHAR) {
//
//			wchar_t* buf = wing_str_char_to_wchar((const char *)_str.data());
//			size_t new_size = WING_WCHAR_SIZE(buf) - sizeof(wchar_t) + this->str_size;
//
//			wchar_t* buffer = (wchar_t*)malloc(new_size);
//			memset(buffer, 0x0, new_size);
//            #ifdef WIN32
//			wsprintfW(buffer, L"%s%s", this->str, buf);
//			#else
//			swprintf(buffer, new_size, L"%s%s", this->str, buf);
//			#endif
//			free(this->str);
//			free(buf);
//
//			this->str      = buffer;
//			this->str_size = new_size;
//			return;
//
//		}
//		return;
//	}


}


/**
 *@�˴�����char* ��Ҫʹ��free�ͷţ����Ҳ��ı�������?
 */
//char* WString::c_str() {
//
//	char* res = NULL;
//
//	switch (this->str_type) {
//
//	case WING_STR_IS_CHAR:
//		{
//			res = (char*)this->copy();
//		}
//		break;
//	case WING_STR_IS_WCHAR:
//		{
//			res = wing_str_wchar_to_char((const wchar_t*)str);
//		}
//		break;
//	}
//
//	return res;
//}
/**
 *@�˴�����wchar_t* ��Ҫʹ��free�ͷţ����Ҳ��ı�������?
 */
//wchar_t* WString::w_str() {
//
//	wchar_t* res = NULL;
//
//	switch (this->str_type) {
//
//	case WING_STR_IS_CHAR:
//		{
//			res = wing_str_char_to_wchar((const char*)this->str);
//		}
//		break;
//	case WING_STR_IS_WCHAR:
//		{
//			res = (wchar_t*)this->copy();
//		}
//		break;
//	}
//
//	return res;
//}
/**
 *@��ӡ�ַ���
 */
void WString::print() {
	 setlocale(LC_ALL, "chs");
	if (this->str_type == WING_STR_IS_CHAR)
		printf("---char,len=%zu,%s---\r\n",this->length(),(char*)this->str);
	else if (this->str_type == WING_STR_IS_WCHAR)
		wprintf(L"---wchar_t,len=%zu,%ls---\r\n",this->length(),(wchar_t *)this->str);
}
/**
 *@��ȫ��ӡ�ַ��������������ݰ�ȫ
 */
void WString::savePrint() {
	setlocale(LC_ALL, "chs");

	long i   = 0;
	long end = this->length();

	if (this->str_type == WING_STR_IS_CHAR) {
		printf("---char,len=%zu,",this->length());
		while(i < end) {
			char c = ((char*)this->str)[i];
			if (c == '\0') c = ' ';
			printf("%c", c);
			i++;
		}
		printf("---\r\n");
	}
	else if (this->str_type == WING_STR_IS_WCHAR) {
		
		wprintf(L"---wchar_t,len=%zu,",this->length());
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
int WString::toUTF8()
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

char* WString::trim()
{
	if (this->str == NULL || this->str_size <= 0) {
	    return NULL;
	}

	if (this->str_type == WING_STR_IS_CHAR) {
	    size_t sub = sizeof(char);
        char *_et  = (char*)((size_t)str + this->str_size - 2*sub);
        char *_st  = (char*)str;

        while (*_et == ' ') {
            *_et-- = '\0';
            this->str_size = this->str_size - sub;
        }

        while (*_st == ' ') {
            while (_st <= _et) {
                *_st = *(_st + 1);
                _st++;
            }
            this->str_size = this->str_size - sub;
            _st = (char*)str;
        }

		return (char*)this->str;
	}

	return NULL;
}

char* WString::ltrim()
{
	if (this->str == NULL || this->str_size <= 0) {
	    return NULL;
	}

	if (this->str_type == WING_STR_IS_CHAR) {
	    size_t sub = sizeof(char);
        char *_et  = (char*)((size_t)this->str + this->str_size - sub);
        char *_st  = (char*)this->str;

        while (*_st == ' ') {
            while (_st <= _et) {
                *_st = *(_st + 1);
                _st++;
            }
            this->str_size = this->str_size - sub;
            _st = (char*)this->str;
        }

		return (char*)this->str;
	}
	return NULL;
}

char* WString::rtrim()
{
	if (this->str == NULL || this->str_size <= 0) {
	    return NULL;
	}

	if (this->str_type == WING_STR_IS_CHAR) {
		char *_et = (char*)((size_t)this->str + this->str_size - 2*sizeof(char));
        while (*_et == ' ') {
            *_et-- = '\0';
            this->str_size = this->str_size - sizeof(char);
        }
		return (char*)this->str;
	}
	return NULL;
}

/***
 * @��ȫ�Ľ��ַ���ת��Ϊdouble����
 */
double WString::toNumber() {
	
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
void* WString::substr(int start,size_t length) {

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

//----WString end------------------------


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
#else
	//int ToWchar(char* &src, wchar_t* &dest, const char *locale = "zh_CN.utf8")
   // {
    if (_str == NULL) {
        return NULL;
    }

      // 根据环境变量设置locale
      setlocale(LC_CTYPE, "zh_CN.utf8");

      // 得到转化为需要的宽字符大小
      int w_size = mbstowcs(NULL, _str, 0) + 1;

      // w_size = 0 说明mbstowcs返回值为-1。即在运行过程中遇到了非法字符(很有可能使locale
      // 没有设置正确)
      if (w_size == 0) {
        return NULL;
      }

     // wcout << "w_size" << w_size << endl;
      wchar_t *dest = (wchar_t *)malloc((sizeof(wchar_t)+1)*w_size);// [w_size];
      if (!dest) {
          return NULL;
      }

      int ret = mbstowcs(dest, _str, strlen(_str)+1);
      if (ret <= 0) {
        return NULL;
      }

      return dest;
    //  return 0;
    //}

//    int main()
//    {
//      char* str = "中国123";
//      wchar_t *w_str ;
//      ToWchar(str,w_str);
//      wcout << w_str[0] << "--" << w_str[1] << "--" << w_str[2];
//      delete(w_str);
//      return 0;
//    }

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
