#include "win_api.h"
/**
 * 获取系统临时路径
 *
 * @param char *buffer 输出结果，这里的buffer使用char数组，如：
 *    char buffer[MAX_PATH];
 *    wing_get_tmp_dir(buffer);
 */
void wing_get_tmp_dir(char *buffer)
{
	GetTempPath(MAX_PATH, buffer);
	if (0 != wing_access(buffer, W_OK)) {
        buffer = NULL;
        return;
    }

    strcpy((char*)(buffer+strlen(buffer)), "/wing_process");

    if (0 == wing_access(buffer, F_OK)) {
        return;
    }

    if (0 == mkdir(buffer, 0777)) {
        return;
    }
    buffer = NULL;
    return;
}

/**
 * @todo 获取进程占用内存
 * @param int process_id
 * @return int 单位为k
 */
unsigned long get_memory(int process_id)
{
	return 0;
}

/**
 * 获取php完整的路径
 *
 * @param const char* command（这里的参数只是一个伪参数，仅为了与各平台的api统一）
 * @return char* 堆内存，需要手动free，如：
 *  char *pap_path = wing_get_command_path("php");
 *  ... //其他操作
 *  free(php_path);
 */
char* wing_get_command_path(const char* command)
{
	char *PHP_PATH = (char*)malloc(MAX_PATH);
	memset(PHP_PATH, 0, MAX_PATH);
	GetModuleFileName(NULL, PHP_PATH, MAX_PATH);

	return PHP_PATH;
}

/**
 * 获取当前进程id
 *
 * @return int
 */
int wing_get_process_id()
{
	return GetCurrentProcessId();
}

/**
 * @todo 杀死进程
 * @param int process_id
 * @return int 0成功 -1失败
 */
int wing_kill(int process_id)
{
	return 0;
}

/**
 * 创建一个新的进程
 *
 * @param const char *command 需要以守护进程运行的指令
 * @param char* output_file 输出重定向到文件，如果不等于NULL，则认为是以守护方式运行
 * @return unsigned long PROCESS_INFORMATION结构体指针（linux下返回进程id）
 */
unsigned long wing_create_process(const char *command, char* output_file)
{
	int redirect_output = output_file == NULL ? 0 : 1;
	STARTUPINFO sui;
	PROCESS_INFORMATION *pi = new PROCESS_INFORMATION();
	SECURITY_ATTRIBUTES sa;

	sa.bInheritHandle       = TRUE;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength              = sizeof(SECURITY_ATTRIBUTES);

	SECURITY_ATTRIBUTES *psa      = NULL;
	DWORD dwShareMode             = FILE_SHARE_READ | FILE_SHARE_WRITE;
	OSVERSIONINFO osVersion       = { 0 };
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
                                    NULL
                                );
	SetLastError(0);
	ZeroMemory(&sui, sizeof(STARTUPINFO));         // ???????????????????ZeroMemory, ????????????memset

	sui.cb         = sizeof(STARTUPINFO);
	sui.dwFlags    = STARTF_USESTDHANDLES;
	sui.hStdInput  = NULL;//m_hRead;
	sui.hStdOutput = hConsoleRedirect;//m_hWrite;
	sui.hStdError  = hConsoleRedirect;//GetStdHandle(STD_ERROR_HANDLE);
									 //sui.wShowWindow = SW_SHOW;
	if (!redirect_output) {
		sui.dwFlags = STARTF_USESHOWWINDOW;// | STARTF_USESTDHANDLES;;
	}

	/*
	if (params_len >0) {
	    DWORD byteWrite  = 0;
	    if (::WriteFile( m_hWrite, params, params_len, &byteWrite, NULL ) == FALSE) {
	        php_error_docref(NULL TSRMLS_CC, E_WARNING, "write data to process error");
	    }
	}
	*/

	if (!CreateProcessA(NULL, (LPSTR)command, NULL, NULL, TRUE, 0, NULL, NULL, &sui, pi)) {
		CloseHandle(hConsoleRedirect);
		return 0;
	}

	CloseHandle(hConsoleRedirect);
	return (unsigned long)pi;
}