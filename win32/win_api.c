#include "win_api.h"

char* wing_get_command_path(const char* command)
{
    char *PHP_PATH = (char*)malloc(MAX_PATH);
    memset(PHP_PATH, 0, MAX_PATH);
    GetModuleFileName(NULL, PHP_PATH, MAX_PATH);

    return PHP_PATH;
}

int wing_get_process_id()
{
    return GetCurrentProcessId();
}

unsigned long get_memory(int process_id)
{
    return 0;
}

int wing_kill(int process_id)
{
    return 0;
}

unsigned long wing_create_process(const char *command, char* output_file)
{
int redirect_output = output_file == NULL ? 0 : 1;
        STARTUPINFO sui;
    	PROCESS_INFORMATION *pi = new PROCESS_INFORMATION(); // �������������ӽ��̵���Ϣ
    	SECURITY_ATTRIBUTES sa;                            // �����̴��ݸ��ӽ��̵�һЩ��Ϣ

    	sa.bInheritHandle       = TRUE;                         // �������ӽ��̼̳и����̵Ĺܵ����
    	sa.lpSecurityDescriptor = NULL;
    	sa.nLength              = sizeof(SECURITY_ATTRIBUTES);

    	SECURITY_ATTRIBUTES *psa = NULL;
    	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    	OSVERSIONINFO osVersion = { 0 };
    	osVersion.dwOSVersionInfoSize = sizeof(osVersion);
    	if (GetVersionEx(&osVersion)) {
    		if (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    			psa = &sa;
    			dwShareMode |= FILE_SHARE_DELETE;
    		}
    	}

    	HANDLE hConsoleRedirect = CreateFile(
    		output_file,
    		GENERIC_WRITE,
    		dwShareMode,
    		psa,
    		OPEN_ALWAYS,
    		FILE_ATTRIBUTE_NORMAL,
    		NULL);
    	SetLastError(0);
    	ZeroMemory(&sui, sizeof(STARTUPINFO));         // ��һ���ڴ������㣬�����ZeroMemory, �����ٶ�Ҫ����memset

    	sui.cb         = sizeof(STARTUPINFO);
    	sui.dwFlags    = STARTF_USESTDHANDLES;
    	sui.hStdInput  = NULL;//m_hRead;
    	sui.hStdOutput = hConsoleRedirect;//m_hWrite;
    	sui.hStdError  = hConsoleRedirect;//GetStdHandle(STD_ERROR_HANDLE);
    	//sui.wShowWindow = SW_SHOW;
    	if(!redirect_output) {
    	    sui.dwFlags = STARTF_USESHOWWINDOW;// | STARTF_USESTDHANDLES;;
    	}
    									 /*if( params_len >0 ) {
    									 DWORD byteWrite  = 0;
    									 if( ::WriteFile( m_hWrite, params, params_len, &byteWrite, NULL ) == FALSE ) {
    									 php_error_docref(NULL TSRMLS_CC, E_WARNING, "write data to process error");
    									 }
    									 }*/
    	if (!CreateProcessA(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &sui, pi)) {
    		CloseHandle(hConsoleRedirect);
    		RETURN_LONG(WING_ERROR_FAILED);
    		return;
    	}
    	CloseHandle(hConsoleRedirect);

//    	zend_update_property_long(wing_process_ce, getThis(), "process_info_pointer", strlen("process_info_pointer"), (zend_long)pi TSRMLS_CC);
//    	//zend_update_property_string(wing_process_ce, getThis(), "command_line", strlen("command_line"), command TSRMLS_CC);
//    	zend_update_property_long(wing_process_ce, getThis(), "process_id", strlen("process_id"), pi->dwProcessId TSRMLS_CC);
//    	zend_update_property_long(wing_process_ce, getThis(), "thread_id", strlen("thread_id"), pi->dwThreadId TSRMLS_CC);
//
    	//RETURN_LONG(pi->dwProcessId);
    	return (unsigned long )pi;
}