#ifndef __WING_WMIC_H__
#define __WING_WMIC_H__
#define _WIN32_DCOM
#include "Windows.h"
#include <comdef.h>
#include <Wbemidl.h>
#include "WingString.class.h"

#pragma comment(lib, "wbemuuid.lib")

class WingWmic{
private:
	IWbemServices *pSvc; 
	int has_error; 
	IEnumWbemClassObject* pEnumerator; 
	IWbemLocator *pLoc; 
	char *query_table;
	IWbemClassObject *pclsObj;
public:
	WingWmic();
	~WingWmic();
	void query( const char *sql );
	BOOL next();
	char* get( const char *key);
};
#endif