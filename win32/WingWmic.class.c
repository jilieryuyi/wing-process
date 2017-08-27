#include "WingWmic.class.h"
#include "OleAuto.h"
#pragma comment(lib,"OleAut32.lib")

WingWmic::WingWmic(){

	this->pSvc			= NULL;
	this->pEnumerator	= NULL;
	this->has_error		= 0;
	this->pLoc			= NULL;
	this->query_table   = NULL;
	this->pclsObj       = NULL;

	HRESULT hres;
	hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
	if (FAILED(hres))
	{
		this->has_error = 1;
		return;             
	}

	hres =  CoInitializeSecurity( NULL, -1,  NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL );

	if (FAILED(hres))
	{
		this->has_error = 1;
		return ;
	}


	hres = CoCreateInstance( CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,  IID_IWbemLocator, (LPVOID *) &pLoc );

	if (FAILED(hres))
	{
		this->has_error = 1;
		return ;            
	}

	hres = pLoc->ConnectServer( _bstr_t(L"ROOT\\CIMV2"), NULL, NULL,  0,  NULL,  0,  0,   &this->pSvc  );

	if (FAILED(hres))
	{   
		this->has_error = 1;
		return ;             
	}

	hres = CoSetProxyBlanket( pSvc,  RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,  NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,  NULL,  EOAC_NONE );

	if (FAILED(hres))
	{    
		this->has_error = 1;
		return;        
	}

}

WingWmic::~WingWmic(){
	if( pSvc != NULL ) 
		pSvc->Release();
	if( pLoc != NULL ) 
		pLoc->Release();
	if( pEnumerator != NULL )
		pEnumerator->Release();
	if( this->query_table != NULL ) 
		delete[] this->query_table;
	if( this->pclsObj != NULL)
		this->pclsObj->Release();
	CoUninitialize();
}


void WingWmic::query( const char* _sql ){

	if( this->has_error ) return;
	/*char *sql = _strdup( _sql );

	int i = 0;
	while( sql[i] != '\0' ){
		sql[i] = tolower(sql[i]);
		i++;
	}

	char *from = strstr( sql , "from" )+4;
	while( 1 ) {
		char c = *from;
		if( !isspace(c) ) break;
		from++;
	}

	//得到查询的是那张表
	this->query_table = new char[32];
	memset( this->query_table , 0 , 32);
	int index = 0;
	while(1) {
		char c = *from;
		if( isspace(c) || c == '\0' ) break;
		this->query_table[index] = c;
		index++;
		from++;
	}*/

	HRESULT hres = pSvc->ExecQuery( bstr_t("WQL"), bstr_t(_sql),WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL,&this->pEnumerator);

	if (FAILED(hres))
	{
		this->has_error = 1;
	}

	//delete[] sql;
}

BOOL WingWmic::next(){

	if( pclsObj != NULL ) {
		pclsObj->Release();
		pclsObj=NULL;
	}

	ULONG uReturn = 0;
	HRESULT hr = pEnumerator->Next( WBEM_INFINITE, 1,  &pclsObj, &uReturn );

	return uReturn;
}




void DecimalToString(VARIANT var,char *&buf)  
{              
               
	UINT64 u64  = (UINT64)var.decVal.Lo64;
	UINT64 iMod = 1; //原文章是int  iMod = 1;，改动一下
	UINT64 ui   = (UINT64)var.decVal.Lo64;       
	int _min    = min(var.decVal.signscale,var.decVal.scale);
	for( int  i = 0; i < _min ; i++ )              
	{                   

		ui /= 10;                  
		iMod *= 10;               
	}               

	UINT64 ud     = (UINT64)var.decVal.Lo64 % iMod;                  
	char  sz0[64] = {0};               
	
	_ui64toa(ui, sz0, 10); 

	char sz1[64] = {0};
	char sz2[64] = {0};                 

	_ui64toa(ud, sz1, 10);               

	buf = (char*)malloc(128);
	memset(buf,0,128);

	int zeroSize = var.decVal.scale-strlen(sz1);
	for( int i=0; i<zeroSize; i++ )    //小数部分左边填充0
	{
		sprintf(sz2, "0%s", sz1);
		memcpy(sz1, sz2,64);
	}                                                                                                  

	if( var.decVal.sign < 128 )              
	{                   

		if (var.decVal.signscale > 0)                   
		{                       
			sprintf(buf, "%s.%s" , sz0, sz1);                   
		}                   

		else                    {                       
			sprintf(buf, "%s" , sz0);                   

		}               
	}               

	else               
	{                   
		if (var.decVal.signscale > 0)                   
		{                       
			sprintf(buf, "-%s.%s" , sz0, sz1);                   
		}                   
		else                    
		{                       
			sprintf(buf, "-%s" , sz0);                   
		}               
	}                 

}  





/**
 * @ 返回值需要使用free释放
 */
char* WingWmic::get( const char *key){

	if( this->has_error ) 
	{
		pEnumerator->Release();
		pEnumerator = NULL;
		return NULL;
	}


	VARIANT vtProp;

	wchar_t *wkey = wing_str_char_to_wchar( key );
	HRESULT hr    = pclsObj->Get( wkey , 0, &vtProp, 0, 0);
	char *res     = NULL;

	if( SUCCEEDED( hr ) && vtProp.bstrVal )
	{	
		//根据不同的类型进行格式化
		switch ( V_VT( &vtProp ) ){  

			case VT_BSTR:   //字符串
				res = wing_str_wchar_to_utf8( (const wchar_t*)vtProp.bstrVal );
				break;
			case VT_LPSTR:  //字符串
				{
					int size = sizeof( strlen((char*)vtProp.pvRecord)+1 );
					res = (char*)malloc(size);
					memset( res, 0, size );
					memcpy( res, vtProp.pvRecord, size-1 );
				}
				break;
			case VT_LPWSTR: //字符串
				{
					res = wing_str_wchar_to_utf8( (const wchar_t*)vtProp.pvRecord );
				}
				break;
			case VT_I1:
			case VT_UI1:
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%d", vtProp.bVal);
				}
				break;

			case VT_I2://短整型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%d", vtProp.iVal);
				}
				break;

			case VT_UI2://无符号短整型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%d", vtProp.uiVal);
				}
				break;

			case VT_INT://整型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%d", vtProp.intVal);
				}
				break;

			case VT_I4: //整型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%d", vtProp.lVal);
				}
				break;

			case VT_I8: //长整型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res,"%ld", vtProp.bVal);
				}
				break;

			case VT_UINT://无符号整型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%u", vtProp.uintVal);
				}
				break;

			case VT_UI4: //无符号整型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%u", vtProp.ulVal);
				}
				break;

			case VT_UI8: //无符号长整型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%u", vtProp.ulVal);
				}
				break;

			case VT_VOID:
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%8x", (unsigned int)vtProp.byref);
				}
				break;

			case VT_R4://浮点型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%.4f", vtProp.fltVal);
				}
				break;

			case VT_R8://双精度型
				{
					res = (char*)malloc(32);
					memset(res,0,32);
					sprintf(res, "%.8f", vtProp.dblVal);
				}
				break;

			case VT_DECIMAL: //小数
			
				DecimalToString(vtProp,res);

				break;

			case VT_CY:
				{
					//vtProp.cyVal.Hi
					//COleCurrency cy = vtProp.cyVal;
					//strValue = cy.Format();
				}
				break;

			case VT_BLOB:
			case VT_BLOB_OBJECT:
			case 0x2011:
				//strValue = "[BLOB]";
				break;

			case VT_BOOL://布尔型
				{
					res = (char*)malloc(5);
					memset(res,0,5);
					sprintf(res,"%s",vtProp.boolVal ? "TRUE" : "FASLE" );
				}
				break;

			case VT_DATE: //日期型
				{
					SYSTEMTIME st = {0};
					res = (char*)malloc(32);
					memset(res,0,32);
					VariantTimeToSystemTime(vtProp.date,&st);
					sprintf(res,"%04d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,   st.wHour, st.wMinute, st.wSecond);
	
				}
				break;

			case VT_NULL://NULL值
				//strValue = "VT_NULL";
				break;

			case VT_EMPTY://空
				//strValue = "";
				break;

			case VT_UNKNOWN://未知类型
			default:
				//strValue = "UN_KNOWN";
				break;
			
		}  

	}

	VariantClear(&vtProp);
	if(wkey) free(wkey);

	return res;
}
