/**
 *@字符串处理
 *@auth yuyi
 *@email 297341015@qq.com
 */
#define _CRT_GETPUTWCHAR_NOINLINE
#include "WingString.class.h"
#include "stdio.h"

WingString::WingString( char *_str, int _size ){
	if( _size <= 0 ) 
		_size = WING_CHAR_SIZE( _str );
	this->str      = malloc( _size );
	this->str_size = _size;
	this->str_type = WING_STR_IS_CHAR;

	memset( this->str, 0, _size );
	memcpy( this->str, _str, _size ); 
}

WingString::WingString( wchar_t *_str, int _size ){
	if( _size <= 0 ) 
		_size = WING_WCHAR_SIZE( _str );
	this->str      = malloc( _size );
	this->str_size = _size;
	this->str_type = WING_STR_IS_WCHAR;

	memset( this->str, 0x0, _size );
	memcpy( this->str, _str, _size ); 
}

WingString::WingString(){
	this->str      = NULL;
	this->str_size = 0;
	this->str_type = WING_STR_IS_UNKNOW;
}

WingString::~WingString(){
	if( this->str != NULL ) {
		free( this->str );
		this->str = NULL;
	}
	this->str_size = 0;
	this->str_type = WING_STR_IS_UNKNOW;
}


BOOL WingString::operator!=( WingString &_str )const{
	return !(*this == _str);
}
BOOL WingString::operator!=( const char* _str )const{
	return !(*this == _str);
}
BOOL WingString::operator!=( const wchar_t* _str )const{
	return !(*this == _str);
}
BOOL WingString::operator<( const wchar_t* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return _str!=NULL;
		break;
	case WING_STR_IS_WCHAR:
		{
				return wcscmp( (wchar_t*)this->str, _str ) < 0;
		}
		break;
	case WING_STR_IS_CHAR:
		{
			wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
			int d = wcscmp( res, _str );
			if( res ) free( res );
			return d < 0;

		}break;
	}
	return false;
}
BOOL WingString::operator<=( const wchar_t* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return 1;
		break;
	case WING_STR_IS_WCHAR:
		{
				return wcscmp( (wchar_t*)this->str, _str ) <= 0;
		}
		break;
	case WING_STR_IS_CHAR:
		{
			wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
			int d = wcscmp( res, _str );
			if( res ) free( res );
			return d <= 0;

		}break;
	}
	return false;
}
BOOL WingString::operator<( const char* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return _str != NULL;
		break;
	case WING_STR_IS_CHAR:
		{
				return strcmp( (char*)this->str, _str ) < 0;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
			int d = strcmp( res, _str );
			if( res ) free( res );
			return d < 0;

		}break;
	}
	return false;
}
BOOL WingString::operator<=( const char* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return 1;
		break;
	case WING_STR_IS_CHAR:
		{
				return strcmp( (char*)this->str, _str ) <= 0;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
			int d = strcmp( res, _str );
			if( res ) free( res );
			return d <= 0;

		}break;
	}
	return false;
}
BOOL WingString::operator<( WingString &_str )const{
	switch( _str.type() ) {
	case WING_STR_IS_UNKNOW:
		return 0;
		break;
	case WING_STR_IS_CHAR:
		{
			if( this->str_type == WING_STR_IS_CHAR ){

				return strcmp( (char*)this->str, (char*)_str.data() ) < 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return _str.type() != WING_STR_IS_UNKNOW;

			}else if( this->str_type == WING_STR_IS_WCHAR ){

				char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
				int d = strcmp( res, (char*)_str.data() );
				if( res ) free( res );
				return d < 0;
			}
			return false;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			if( this->str_type == WING_STR_IS_WCHAR ){

				return wcscmp( (wchar_t*)this->str, (wchar_t*)_str.data() ) < 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return _str.type() != WING_STR_IS_UNKNOW;

			}else if( this->str_type == WING_STR_IS_CHAR ){

				wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
				int d = wcscmp( res, (wchar_t*)_str.data() );
				if( res ) free( res );
				return d < 0;
			}
			return false;

		}break;
	}
	return false;
}
BOOL WingString::operator<=( WingString &_str )const{
	switch( _str.type() ) {
	case WING_STR_IS_UNKNOW:
		return this->str_type == WING_STR_IS_UNKNOW;
		break;
	case WING_STR_IS_CHAR:
		{
			if( this->str_type == WING_STR_IS_CHAR ){

				return strcmp( (char*)this->str, (char*)_str.data() ) <= 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return 1;

			}else if( this->str_type == WING_STR_IS_WCHAR ){

				char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
				int d = strcmp( res, (char*)_str.data() );
				if( res ) free( res );
				return d <= 0;
			}
			return false;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			if( this->str_type == WING_STR_IS_WCHAR ){

				return wcscmp( (wchar_t*)this->str, (wchar_t*)_str.data() ) <= 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return 1;

			}else if( this->str_type == WING_STR_IS_CHAR ){

				wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
				int d = wcscmp( res, (wchar_t*)_str.data() );
				if( res ) free( res );
				return d <= 0;
			}
			return false;

		}break;
	}
	return false;
}
BOOL WingString::operator>( const wchar_t* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return 0;
		break;
	case WING_STR_IS_WCHAR:
		{
				return wcscmp( (wchar_t*)this->str, _str ) > 0;
		}
		break;
	case WING_STR_IS_CHAR:
		{
			wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
			int d = wcscmp( res, _str );
			if( res ) free( res );
			return d > 0;

		}break;
	}
	return false;
}
BOOL WingString::operator>=( const wchar_t* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return _str == NULL;
		break;
	case WING_STR_IS_WCHAR:
		{
				return wcscmp( (wchar_t*)this->str, _str ) >= 0;
		}
		break;
	case WING_STR_IS_CHAR:
		{
			wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
			int d = wcscmp( res, _str );
			if( res ) free( res );
			return d >= 0;

		}break;
	}
	return false;
}
BOOL WingString::operator>( const char* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return 0;
		break;
	case WING_STR_IS_CHAR:
		{
				return strcmp( (char*)this->str, _str ) > 0;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
			int d = strcmp( res, _str );
			if( res ) free( res );
			return d > 0;

		}break;
	}
	return false;
}
BOOL WingString::operator>=( const char* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return _str == NULL;
		break;
	case WING_STR_IS_CHAR:
		{
				return strcmp( (char*)this->str, _str ) >= 0;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
			int d = strcmp( res, _str );
			if( res ) free( res );
			return d >= 0;

		}break;
	}
	return false;
}
BOOL WingString::operator>=( WingString &_str )const{
	switch( _str.type() ) {
	case WING_STR_IS_UNKNOW:
		return 1;
		break;
	case WING_STR_IS_CHAR:
		{
			if( this->str_type == WING_STR_IS_CHAR ){

				return strcmp( (char*)this->str, (char*)_str.data() ) >= 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return 0;

			}else if( this->str_type == WING_STR_IS_WCHAR ){

				char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
				int d = strcmp( res, (char*)_str.data() );
				if( res ) free( res );
				return d >= 0;
			}
			return false;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			if( this->str_type == WING_STR_IS_WCHAR ){

				return wcscmp( (wchar_t*)this->str, (wchar_t*)_str.data() ) >= 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return 0;

			}else if( this->str_type == WING_STR_IS_CHAR ){

				wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
				int d = wcscmp( res, (wchar_t*)_str.data() );
				if( res ) free( res );
				return d >= 0;
			}
			return false;

		}break;
	}
	return false;
}
BOOL WingString::operator>( WingString &_str )const{
	switch( _str.type() ) {
	case WING_STR_IS_UNKNOW:
		return this->str_type != WING_STR_IS_UNKNOW;
		break;
	case WING_STR_IS_CHAR:
		{
			if( this->str_type == WING_STR_IS_CHAR ){

				return strcmp( (char*)this->str, (char*)_str.data() ) > 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return 0;

			}else if( this->str_type == WING_STR_IS_WCHAR ){

				char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
				int d = strcmp( res, (char*)_str.data() );
				if( res ) free( res );
				return d > 0;
			}
			return false;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			if( this->str_type == WING_STR_IS_WCHAR ){

				return wcscmp( (wchar_t*)this->str, (wchar_t*)_str.data() ) > 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return 0;

			}else if( this->str_type == WING_STR_IS_CHAR ){

				wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
				int d = wcscmp( res, (wchar_t*)_str.data() );
				if( res ) free( res );
				return d > 0;
			}
			return false;

		}break;
	}
	return false;
}
BOOL WingString::operator==( const wchar_t* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return _str == NULL;
		break;
	case WING_STR_IS_WCHAR:
		{
			return wcscmp( (wchar_t*)this->str, _str ) == 0;
		}
		break;
	case WING_STR_IS_CHAR:
		{
			char *res = wing_str_wchar_to_char( _str );
			int d = strcmp( (char*)this->str, res );
			if( res ) free( res );
			return d == 0;
			
		}break;
	}
	return false;
}
BOOL WingString::operator==( const char* _str )const{
	switch( this->str_type ) {
	case WING_STR_IS_UNKNOW:
		return _str == NULL;
		break;
	case WING_STR_IS_CHAR:
		{
			return strcmp( (char*)this->str, _str ) == 0;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			wchar_t *res = wing_str_char_to_wchar( _str );
			int d = wcscmp( (wchar_t*)this->str, res );
			if( res ) free( res );
			return d == 0;
			
		}break;
	}
	return false;
}
BOOL WingString::operator==( WingString &_str )const{
	switch( _str.type() ) {
	case WING_STR_IS_UNKNOW:
		return this->str_type == WING_STR_IS_UNKNOW;
		break;
	case WING_STR_IS_CHAR:
		{
			if( this->str_type == WING_STR_IS_CHAR ){

				return strcmp( (char*)this->str, (char*)_str.data() ) == 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return false;

			}else if( this->str_type == WING_STR_IS_WCHAR ){

				char *res = wing_str_wchar_to_char( (wchar_t*)this->str );
				int d = strcmp( (char*)_str.data(), res );
				if( res ) free( res );
				return d == 0;
			}
			return false;
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			if( this->str_type == WING_STR_IS_WCHAR ){

				return wcscmp( (wchar_t*)this->str, (wchar_t*)_str.data() ) == 0;

			}else if( this->str_type == WING_STR_IS_UNKNOW ) {

				return false;

			}else if( this->str_type == WING_STR_IS_CHAR ){

				wchar_t *res = wing_str_char_to_wchar( (char*)this->str );
				int d = wcscmp( (wchar_t*)_str.data(), res );
				if( res ) free( res );
				return d == 0;
			}
			return false;

		}break;
	}
	return false;
}
WingString &WingString::operator=(const char* _str){
	 this->append( _str , WING_CHAR_SIZE(_str) );
     return *this;
 }
WingString &WingString::operator=(const wchar_t* _str){
	 this->append( _str , WING_WCHAR_SIZE(_str) );
     return *this;
 }
WingString &WingString::operator=(WingString &_str ){
	 this->str		= _str.copy();
	 this->str_size = _str.size();
	 this->str_type = _str.type();
     return *this;
 }
WingString &WingString::operator+(WingString &_str ){
	  switch(_str.type()){
	  case WING_STR_IS_UNKNOW :
		  break;
	  case WING_STR_IS_CHAR:
		  this->append( (const char*)_str.data(), _str.size() );
		  break;
	  case WING_STR_IS_WCHAR:
		  this->append( (const wchar_t*)_str.data(), _str.size() );
		  break;
	  }
     return *this;
 }
WingString &WingString::operator+( const char* _str ){ 
	this->append( _str, WING_CHAR_SIZE( _str ) );	  
    return *this;
}
WingString &WingString::operator+( const wchar_t* _str ){ 
	this->append( _str, WING_WCHAR_SIZE( _str ) );	  
    return *this;
}
WingString &WingString::operator+=(WingString &_str ){
	  switch(_str.type()){
	  case WING_STR_IS_UNKNOW :
		  break;
	  case WING_STR_IS_CHAR:
		  this->append( (const char*)_str.data(), _str.size() );
		  break;
	  case WING_STR_IS_WCHAR:
		  this->append( (const wchar_t*)_str.data(), _str.size() );
		  break;
	  }
     return *this;
 }
WingString &WingString::operator+=( const char* _str ){ 
	this->append( _str, WING_CHAR_SIZE( _str ) );	  
    return *this;
}
WingString &WingString::operator+=( const wchar_t* _str ){ 
	this->append( _str, WING_WCHAR_SIZE( _str ) );	  
    return *this;
}

unsigned int WingString::size(){
	return this->str_size;
}
unsigned int WingString::length(){
	
	switch( this->str_type )
	{
		case WING_STR_IS_CHAR:
			return (unsigned int)(  this->str_size/sizeof(char)  -1 );
			break;
		case WING_STR_IS_WCHAR:
			return (unsigned int)(  this->str_size/sizeof(wchar_t) -1 );
			break;
		default:
			return 0;
	}
	return 0;
}

/**
 *@追加字符串
 */
void WingString::append( const wchar_t *_str, int size ){
		
	if( _str == NULL )
    {
		return;
    }

	if( size <=0 ) 
		size = WING_WCHAR_SIZE( _str );

	if( this->str_type == WING_STR_IS_UNKNOW )
	{
		this->str_type = WING_STR_IS_WCHAR;
		this->str      = malloc( size );
		this->str_size = size;

		memcpy( this->str, _str, size );
		return;
	}

	if( this->str_type == WING_STR_IS_CHAR ){
		
		char *res = wing_str_wchar_to_char( (const wchar_t*)_str );

		int len       = WING_CHAR_SIZE( res );
		int new_len   = this->str_size + len - 1 ;

		char *new_str = (char*)malloc(new_len);

		memset( new_str , 0 , new_len );

		char *str_begin = new_str;
		memcpy( str_begin , this->str , this->str_size - 1 );

		str_begin += (this->str_size - 1);
		memcpy( str_begin , res , len );

		free( this->str );
		free( res );

		this->str      = new_str;
		this->str_size = new_len;
		return;
	}

	if( this->str_type == WING_STR_IS_WCHAR ) {
	    
		int wl       = sizeof(wchar_t);
		int new_size = this->str_size + size - wl;
		
		wchar_t* res = (wchar_t*)malloc( new_size );

		memset( res, 0x0, new_size );

		wsprintfW( res, L"%s%s", this->str, _str );
	
		free( this->str );

		this->str      = res;
		this->str_size = new_size;
		return;
	}

}
/**
 *@追加字符串
 */
void WingString::append( const char *_str, int size ){
		
	if( _str == NULL )
    {
		return;
    }

	if( size <=0 ) 
		size = WING_CHAR_SIZE( _str );

	//如果构造的时候 没有初始化
	if( this->str_type == WING_STR_IS_UNKNOW )
	{
		this->str_type = WING_STR_IS_CHAR;
		this->str      = malloc( size );
		this->str_size = size;

		memcpy( this->str, _str, size );
		return;
	}

	if( this->str_type == WING_STR_IS_CHAR ){
		
		int new_size = this->str_size - 1 + size;
		char *res = (char*)malloc( new_size );
		memset( res, 0, new_size );

		char *str_start = res;
		memcpy( str_start, this->str, this->str_size - 1 );
		str_start +=  this->str_size - 1;

		memcpy( str_start, _str, size );

		free( this->str );

		this->str      = res;
		this->str_size = new_size;

		return;
	}

	if( this->str_type == WING_STR_IS_WCHAR ) {

		wchar_t* buf = wing_str_char_to_wchar( (const char *)_str );
		int new_size = WING_WCHAR_SIZE( buf ) - sizeof(wchar_t) + this->str_size;

		wchar_t* buffer = (wchar_t*)malloc(new_size);
		memset( buffer, 0x0, new_size );

		wsprintfW( buffer, L"%s%s", this->str, buf );
		free( this->str );
		free( buf );

		this->str      = buffer;
		this->str_size = new_size;
	}

}
/**
 *@追加字符串
 */
void WingString::append( WingString &_str ){
		
	if( this->str_type == WING_STR_IS_UNKNOW ) {

		int size       = _str.size();
		this->str      = malloc( size );
		this->str_size = size;
		this->str_type = _str.type();

		memset( this->str, 0 , size );
		memcpy( this->str, _str.data(), size );
		return;
	}

	else if( this->str_type == WING_STR_IS_CHAR ){

		if( _str.type() == WING_STR_IS_UNKNOW )
			return;

		else if( _str.type() == WING_STR_IS_CHAR ){
			
			int new_size = this->str_size - 1 + _str.size();
			char *res = (char*)malloc( new_size );
			memset( res, 0, new_size );

			char *str_start = res;
			memcpy( str_start, this->str, this->str_size - 1 );
			str_start +=  this->str_size - 1;

			memcpy( str_start, _str.data(), _str.size() );

			free( this->str );

			this->str      = res;
			this->str_size = new_size;

			return;
		}
		else if( _str.type() == WING_STR_IS_WCHAR ){

			char *res = wing_str_wchar_to_char( (const wchar_t*)_str.data() );

			int len       = WING_CHAR_SIZE( res );
			int new_len   = this->str_size + len - 1 ;

			char *new_str = (char*)malloc(new_len);

			memset( new_str , 0 , new_len );

			char *str_begin = new_str;
			memcpy( str_begin , this->str , this->str_size - 1 );

			str_begin += (this->str_size - 1);
			memcpy( str_begin , res , len );

			free( this->str );
			free( res );

			this->str      = new_str;
			this->str_size = new_len;
			return;

		}
		return;
	}

	else if( this->str_type == WING_STR_IS_WCHAR ){
		
		if( _str.type() == WING_STR_IS_UNKNOW )
			return;

		else if( _str.type() == WING_STR_IS_WCHAR ) {
			
			int wl       = sizeof(wchar_t);
			int new_size = this->str_size + _str.size() - wl;
		
			wchar_t* res = (wchar_t*)malloc( new_size );

			memset( res, 0x0, new_size );

			wsprintfW( res, L"%s%s", this->str, _str.data() );
	
			free( this->str );

			this->str      = res;
			this->str_size = new_size;
			return;
		}

		else if( _str.type() == WING_STR_IS_CHAR ) {
			
			wchar_t* buf = wing_str_char_to_wchar( (const char *)_str.data() );
			int new_size = WING_WCHAR_SIZE( buf ) - sizeof(wchar_t) + this->str_size;

			wchar_t* buffer = (wchar_t*)malloc(new_size);
			memset( buffer, 0x0, new_size );

			wsprintfW( buffer, L"%s%s", this->str, buf );
			free( this->str );
			free( buf );

			this->str      = buffer;
			this->str_size = new_size;
			return;

		}
		return;
	}


}
/**
 *@原型数据
 */
void * WingString::data(){
	return this->str;
}
/**
 *@字符串编码类型 char 还是 wachr_t 还是 unkonw 返回值
 *@ WING_STR_IS_CHAR   1
 *@ WING_STR_IS_WCHAR  2
 *@ WING_STR_IS_UNKNOW 3
 */
int WingString::type(){
	return this->str_type;
}
/**
 *@拷贝数据
 */
void* WingString::copy(){
	if( str == NULL || str_size <= 0 )
		return NULL;
	void* res = malloc(str_size);
	memset(res,0,str_size);
	memcpy(res,str,str_size);
	return res;
}

/**
 *@此处返回char* 需要使用free释放，并且不改变自身的值
 */
char* WingString::c_str(){

	char* res = NULL;

	switch( this->str_type ) {
	
	case WING_STR_IS_CHAR:
		{
			res = (char*)this->copy();
		}
		break;
	case WING_STR_IS_WCHAR:
		{
			res = wing_str_wchar_to_char( (const wchar_t*)str );	   
		}
		break;
	}

	return res;
}
/**
 *@此处返回wchar_t* 需要使用free释放，并且不改变自身的值
 */
wchar_t* WingString::w_str(){

	wchar_t* res = NULL;

	switch( this->str_type ) {
	
	case WING_STR_IS_CHAR:
		{
			res = wing_str_char_to_wchar( (const char*)this->str );
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
 *@打印字符串
 */
void WingString::print(){
	 setlocale(LC_ALL, "chs");
	if( this->str_type == WING_STR_IS_CHAR )
		printf("---char:size=%ld,len=%ld,%s---\r\n",this->size(),this->length(),this->str);
	else if( this->str_type == WING_STR_IS_WCHAR )
		wprintf(L"---wchar_t:size=%ld,len=%ld,%s---\r\n",this->size(),this->length(),this->str);
}
/**
 *@安全打印字符串，二进制数据安全
 */
void WingString::savePrint(){
	setlocale(LC_ALL, "chs");

	long i   = 0;
	long end = this->length();

	if( this->str_type == WING_STR_IS_CHAR ) {
		printf("---char:size=%ld,len=%ld,",this->size(),this->length());
		while( i < end ){
			char c = ((char*)this->str)[i];
			if( c == '\0') c = ' ';
			printf("%c", c );
			i++;
		}
		printf("---\r\n");
	}
	else if( this->str_type == WING_STR_IS_WCHAR ) {
		
		wprintf(L"---wchar_t:size=%ld,len=%ld,",this->size(),this->length());
		while( i < end ){
			wprintf(L"%c",((wchar_t*)this->str)[i]);
			i++;
		}
		
		wprintf(L"---\r\n");
	}
}


/**
 * @字符串转换为utf8编码，会改变自身
 */
BOOL WingString::toUTF8()
{
	char *utf8_str = NULL;
	switch( this->str_type ){
	case WING_STR_IS_CHAR:
		{
			utf8_str = wing_str_char_to_utf8( ( const char* )str );
		}
		break;
	case WING_STR_IS_UNKNOW:
		return 1;
		break;
	case WING_STR_IS_WCHAR:
		{
			utf8_str = wing_str_wchar_to_utf8( (const wchar_t*)this->str );
		}
		break;
	}

	if( utf8_str != NULL ){
		free( str );
		this->str      = utf8_str;
		this->str_size = WING_CHAR_SIZE( utf8_str );
		this->str_type = WING_STR_IS_CHAR;
		return 1;
	}else
		return 0;
}

/**
 *@去除两端空格 ，会改变自身
 */
void WingString::trim(){
	
	if( this->str == NULL || this->str_size <= 0 ) 
		return;
	if( this->str_type == WING_STR_IS_CHAR )
	{
		wing_str_trim( (char*)this->str );
		this->str_size = strlen((char*)this->str )+1;
	}
}

/***
 * @安全的将字符串转换为double数字
 */
double WingString::toNumber(){
	
	//0的ascii为48 
	//9的ascii为57 
	//.的ascii码为46
	//-的ascii码为45
	
	char *numstr  = NULL;
	int need_free = 0;
	switch( this->str_type ){
		case WING_STR_IS_UNKNOW:
			return 0;
			break;
		case WING_STR_IS_CHAR:
			numstr = (char*)this->str;
			break;
		case WING_STR_IS_WCHAR:
			{
				numstr = wing_str_wchar_to_char( (const wchar_t*)this->str );
				if(numstr){
					need_free = 1;
				}else
					return 0;
			}
			break;
	}
	
	int i          = 0;
	int len        = this->length();
	int is_minus   = 0;
	int is_decimal = 0;

	
	double result = 0;
	int maxmi     = 0;
	int haspoint  = 0;

	//这里是为了得到最大的幂和数字长度
	while( i < len ) {
		int ascii = (int)numstr[i];
		if( (ascii < 48 || ascii > 57 ) && ascii != 45 && ascii != 46) break;
		if( (i+1)<len ){
			if( ( ascii==45 || ascii == 46 ) && ( (int)numstr[i+1] == 45 || (int)numstr[i+1] == 46) ) break;
		}
		if( (ascii >= 48 && ascii <= 57 ) && !haspoint ) {
			maxmi++;
			
		} 
		if( ascii == 46 )
			haspoint=1;
		i++;
	}

	if( i <= 0 ) return 0;

	int start  = 0;
	    len    = i;
	double ten = 10;

	//如果是负数
	if( (int)numstr[start] == 45 ){
		start = 1;
		while( start < i ){

			if( (int)numstr[start] == 46 ) {
				start++; 
				continue;
			}

			int m =maxmi-1;
			result -= ( (int)numstr[start]-48 )*pow(ten,m);

			start++;
			maxmi--;
		}

		if( need_free) 
			free(numstr);
		return result;
	}

	

	
	start = 0;
	while( start < i ){

		if( (int)numstr[start] == 46 ) {
			start++; 
			continue;
		}

		int m   = maxmi-1;
		result += ( (int)numstr[start]-48 )*pow(ten,m);

		start++;
		maxmi--;
	}

	if( need_free ) 
		free(numstr);
	
	return result;
}

/**
 *@返回子字符串 不改变字符串本身  
 *@用完之后 返回值 如果返回值不为null 需要 free ,start 从0开始，也可以是负数，从末尾开始截取
 */
void* WingString::substr(int start,int length) {

	int len = this->length();
	if( this->str_type == WING_STR_IS_UNKNOW ) 
		return NULL;
	
	unsigned long sl = 0;
	if( this->str_type == WING_STR_IS_CHAR )
		sl = sizeof(char);
	else if( this->str_type == WING_STR_IS_WCHAR )
		sl = sizeof(wchar_t);

	unsigned long end_str   = (unsigned long)this->str + this->str_size - 1*sl;
	unsigned long start_str = NULL;

	if( start >= 0 ){
		start_str =  (unsigned long)this->str + start*sl ;
		if( start_str >= end_str ) 
			return NULL;
	}else{
		start_str = (unsigned long)this->str + this->str_size + (start - 1)*sl;
		if( start_str < (unsigned long)this->str )
			start_str = (unsigned long)this->str;
	}

	if( (unsigned long)length > (end_str-start_str)/sl ) 
		length = (end_str-start_str)/sl;

	void* _subatr = malloc( (length+1)*sl );
	memset( _subatr, 0 , (length+1)*sl );
	memcpy( _subatr, (const void*)start_str, length*sl );

	return _subatr;
}

//----WingString end------------------------


/**
 *@wchar_t转换编码为utf8
 */
char* wing_str_wchar_to_utf8( _In_ const wchar_t* _str ){
	if( _str == NULL )
		return NULL;
	int nLen = WideCharToMultiByte(CP_UTF8, 0, _str, -1, NULL, 0, NULL, NULL);
	if( nLen <= 0 ) return NULL;
	char*  m_cUtf8 = (char*)malloc(nLen + 1);  
	memset(m_cUtf8,0,nLen + 1);
	WideCharToMultiByte (CP_UTF8, 0, _str, -1, m_cUtf8, nLen, NULL,NULL); 
	return m_cUtf8;

}

/**
 *@wchar_t 转换为 char
 */
char* wing_str_wchar_to_char( _In_ const wchar_t* _str ){
	 if( _str == NULL )
		 return NULL;
	 int nLen = WideCharToMultiByte(CP_OEMCP,NULL, _str,-1,NULL,0,NULL,FALSE);  
	 if( nLen <= 0 ) return NULL;
     char * m_cDest = (char*)malloc(nLen); 
	 memset(m_cDest,0,nLen);
     WideCharToMultiByte (CP_OEMCP,NULL,_str,-1, m_cDest, nLen,NULL,FALSE);  
	 return m_cDest;	
}
wchar_t* wing_str_char_to_wchar( _In_ const char* _str ){
	
	int size     = WING_CHAR_SIZE( _str );
	int len      = MultiByteToWideChar(CP_ACP,0,(const char *)_str,size-1,NULL,0);

	int buf_size = (len+1)*sizeof(wchar_t);
	wchar_t* buf = (wchar_t*)malloc( buf_size );
	memset( buf, 0x0, buf_size );
	MultiByteToWideChar( CP_ACP,0,(const char *)_str,size-1,buf,len);   

	return buf;
}
char* wing_str_char_to_utf8( _In_ const char* str ){
	
	if( str == NULL )
		return NULL;

	wchar_t* unicode_str = NULL;
	int utf8_str_size    = 0;

	utf8_str_size      = ::MultiByteToWideChar( CP_ACP, 0, str, -1, NULL, NULL );                   //获取转换到Unicode编码后所需要的字符空间长度
	unicode_str        = (wchar_t*)malloc((utf8_str_size + 1)*sizeof(wchar_t));                     //为Unicode字符串空间
	memset( unicode_str, 0x0, (utf8_str_size + 1)*sizeof(wchar_t) );
	utf8_str_size      = ::MultiByteToWideChar( CP_ACP, 0, str, -1, unicode_str, utf8_str_size );   //转换到Unicode编码
	
	if( !utf8_str_size )                                                                                 //转换失败则出错退出
	{
		if( unicode_str ) 
			delete[] unicode_str;
		return 0;
	}

	utf8_str_size  = WideCharToMultiByte(CP_UTF8,0,unicode_str,-1,NULL,0,NULL,NULL);                    //获取转换到UTF8编码后所需要的字符空间长度
	char *utf8_str = (char*)malloc(utf8_str_size+1);

	memset(utf8_str,0,utf8_str_size+1);

	utf8_str_size = WideCharToMultiByte( CP_UTF8, 0, unicode_str, -1, (char *)utf8_str, utf8_str_size+1, NULL, NULL );  
	                                                                                                    //转换到UTF8编码
	if( unicode_str )
		delete []unicode_str;

	if( !utf8_str_size )
		return 0;

	return utf8_str;
}

/**
 *@去除字符串两端空格
 */
void wing_str_trim( _Inout_ char* str ,int size ){
	if( str == NULL ) 
		return;
	if( size <= 0 )
		size = strlen( str );
	
	int len     = size;
	char *start = str;  
    char *end   = str + len - 1;  
  
	//找到第一个不为空的字符
    while (1)   
    {     
        char c = *start;  
        if (!isspace(c))  
            break;  
  
        start++;  
        if (start > end)  
        {     
            str[0] = '\0';  
            return;  
        }    
    }     
  
	//找到最后一个不为空的字符
    while (1)   
    {     
        char c = *end;  
        if ( !isspace(c) && c != '\0' )  //兼容传入的是size或是strlen
            break;  
  
        end--;  
        if (start > end)  
        {     
            str[0] = '\0';  
            return;  
        }  
    }  
  
	//复制区间
    memmove(str, start, end - start + 1);  
	//最后一个值清零
    str[end - start + 1] = '\0';  
}
