/**
 *@进程搜索、枚举
 */
#include "Windows.h"
#include "ntdll.h"
#include "wing_ntdll.h"
#include "tlhelp32.h"
#include "Psapi.h"
#include "WingString.class.h"
#include "WingWmic.class.h"
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Kernel32.lib")

NTSTATUS WingOpenProcess(
	_In_ HMODULE	hNtDll,
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ HANDLE ProcessId
)
{
    NTSTATUS          status;
    OBJECT_ATTRIBUTES objectAttributes;
    CLIENT_ID         clientId;

    clientId.UniqueProcess = ProcessId;
    clientId.UniqueThread  = NULL;

	NTOPENPROCESS NtOpenProcess = (NTOPENPROCESS)GetProcAddress( hNtDll, "NtOpenProcess" );

	InitializeObjectAttributes(&objectAttributes, NULL, 0, NULL, NULL);
    
	status = NtOpenProcess( ProcessHandle, DesiredAccess, &objectAttributes, &clientId );
    
    return status;
}


NTSTATUS WingQueryProcessVariableSize(
	_In_ HMODULE	hNtDll,
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _Out_ PVOID *Buffer
)
{
    NTSTATUS status;
    PVOID    buffer;
    ULONG    returnLength = 0;


	NTQUERYINFORMATIONPROCESS NtQueryInformationProcess = (NTQUERYINFORMATIONPROCESS)GetProcAddress( hNtDll, "NtQueryInformationProcess" );

    status = NtQueryInformationProcess( ProcessHandle, ProcessInformationClass, NULL, 0, &returnLength );

    if ( status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL && status != STATUS_INFO_LENGTH_MISMATCH )
		return status;

    buffer = ( LPVOID )malloc( returnLength );
	memset(buffer,0,returnLength);
    status = NtQueryInformationProcess( ProcessHandle, ProcessInformationClass, buffer, returnLength, &returnLength );

    if ( NT_SUCCESS( status ) )
    {
        *Buffer = buffer;
    }
    else
    {
        free(buffer);
    }
    return status;
}
	



void WingGetProcessCommandLine( DWORD process_id, char* &lpszCommandLine ){ 

	char *empty_string = "\0";
		HANDLE hProcess = ::OpenProcess( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, process_id );
	    HANDLE hToken;
	
		//无法正常OpenProcess的进程跳过 这些进程一般都是系统进程 无权限导致 忽略
		if( !hProcess )
		{
			lpszCommandLine = new char[4];
			//sprintf(lpszCommandLine,"%s",empty_string);
			sprintf_s(lpszCommandLine,4,"%s",empty_string);
			return;
		}
 
		//提升权限
		if( OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES,&hToken ) )
	    {
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			
			LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid);
			
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges( hToken, FALSE, &tp, sizeof(tp), NULL, NULL );
			CloseHandle( hToken );
	    }

        DWORD dwThreadId;
		HANDLE hThread  = ::CreateRemoteThread( hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)GetCommandLine, NULL, 0, &dwThreadId);
 
		//无法正常CreateRemoteThread 的进程跳过 这些进程一般都是无权限导致 忽略
		if( !hThread )
		{
			lpszCommandLine = new char[4];
			sprintf_s(lpszCommandLine,4,"%s",empty_string);
			CloseHandle( hProcess );
			return;
		}
	
		DWORD dwExitCode        = 0;
		SIZE_T dwReaded          = 0;
		int buf_size            = 10240;
		lpszCommandLine         = new char[buf_size]; 
        
		memset(lpszCommandLine,0,buf_size);
        
		::WaitForSingleObject(hThread, 500);
        ::GetExitCodeThread(hThread, &dwExitCode);

		SetLastError(0);
		::ReadProcessMemory(hProcess, (LPCVOID)dwExitCode, lpszCommandLine, sizeof(char)*buf_size, &dwReaded);

		//进程的可执行文件名称。要获得可执行文件的完整路径，应调用Module32First函数，再检查其返回的MODULEENTRY32结构的szExePath成员。
		//但是，如果被调用进程是一个64位程序，您必须调用QueryFullProcessImageName函数去获取64位进程的可执行文件完整路径名。

	
        int start          = 10240;


		if( 299 == GetLastError() ) 
		{
		   
			delete[] lpszCommandLine;
			lpszCommandLine = NULL;
		
		    //299失败这部分处理起来有点麻烦，不知道有没有更好的方法
			//这里说一下为什么会299失败，一般都是缓冲区太大了
			//也就是ReadProcessMemory的第四个参数，这个参数太大导致目标进程中读取超出范围，然后读到了一些无法读取的内存造成失败
			
			
			while( 1 ) {

				lpszCommandLine = new char[start];
				memset( lpszCommandLine, 0, start );

				//这句也很重要哦 重置错误
				SetLastError(0);

				::ReadProcessMemory(hProcess, (LPCVOID)dwExitCode, lpszCommandLine, sizeof(char)*start, &dwReaded );

				if(  299 == GetLastError() ) 
				{
					//失败处理坑爹就坑在这里啦，谁有更好的方法记得分享 email 297341015@qq.com
					delete[] lpszCommandLine;
					lpszCommandLine = NULL;
					start--;
					if( start<= 0 ) break;
				}else
				{
					break;
				}
			}
		}

		if( lpszCommandLine == NULL ){
			lpszCommandLine = new char[4];
			sprintf_s(lpszCommandLine,4,"%s",empty_string);
		}
		CloseHandle( hProcess );
}


/**
 *@自身进程相关信息查询
 */
DWORD WingGetCurrentProcessInfo( PROCESSINFO *&all_process, int from, int max_count , int has_current_process ){
	
	DWORD process_id = GetCurrentProcessId();
	HANDLE handle = INVALID_HANDLE_VALUE;
	PROCESS_MEMORY_COUNTERS pmc;
    PROCESSENTRY32 pe32; 
    pe32.dwSize         = sizeof(pe32);   
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if( hProcessSnap == INVALID_HANDLE_VALUE)  
    {  
		return 0;
    }  

    BOOL bMore = ::Process32First( hProcessSnap, &pe32 );  

	MODULEENTRY32 me32;
	me32.dwSize = sizeof( MODULEENTRY32 );


	HANDLE hModuleSnap = INVALID_HANDLE_VALUE , hProcess = INVALID_HANDLE_VALUE;
	char *exepath      = NULL;
	DWORD outsize      = 0;
	int dwTotalProcess = 0;

	
	HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
    while( bMore )  
    {  
		if(has_current_process){
			if(pe32.th32ProcessID == process_id) {
				bMore = ::Process32Next(hProcessSnap, &pe32);
				continue;
			}
		}
		
		if( pe32.th32ProcessID != process_id && pe32.th32ParentProcessID != process_id )
		{
			bMore = ::Process32Next(hProcessSnap, &pe32);
			continue;
		}

		if( all_process == NULL ) {
			dwTotalProcess++;
			bMore = ::Process32Next(hProcessSnap, &pe32);
			continue;
		}

		
		PROCESSINFO *process_item = &all_process[from+dwTotalProcess];
		
		hModuleSnap = ::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pe32.th32ProcessID ); 
		ZeroMemory(&me32,me32.dwSize);

		me32.dwSize = sizeof( MODULEENTRY32 );

		if( hModuleSnap != INVALID_HANDLE_VALUE )
		{
			Module32First( hModuleSnap , &me32 ); //QueryFullProcessImageName GetProcessImageFileName
			CloseHandle( hModuleSnap );
			hModuleSnap = INVALID_HANDLE_VALUE;
		}
		SetLastError(0);
	
		process_item->process_id        = pe32.th32ProcessID ;
		
		int size = sizeof(pe32.szExeFile)+1;
		process_item->process_name      = new char[size];
		memset(process_item->process_name,0,size);
		sprintf_s(process_item->process_name,size,"%s",pe32.szExeFile);

		process_item->parent_process_id = pe32.th32ParentProcessID;

		size= sizeof(me32.szExePath)+1;
		process_item->file_path         = new char[size];//me32.szExePath;
		memset(process_item->file_path,0,size);
		sprintf_s(process_item->file_path,size,"%s",me32.szExePath);
		
		process_item->cpu_time          = 0;

		size= sizeof(pe32.szExeFile)+1;
		process_item->file_name         = new char[size];
		memset(process_item->file_name,0,size);
		sprintf_s(process_item->file_name,size,"%s",pe32.szExeFile);

		 
		process_item->thread_count      = 0;
		process_item->base_priority     = 0;
		process_item->handle_count      = 0;
		process_item->working_set_size  = 0;

		//process_item->command_line      = new char[4];
		//memset( process_item->command_line,0,4);

		if( process_id == pe32.th32ProcessID ) {
			handle = GetCurrentProcess();	
			char *comm = GetCommandLineA();
			size = strlen(comm)+1;
			process_item->command_line = new char[size];
			memset(process_item->command_line,0,size);
			sprintf_s(process_item->command_line,size,"%s",comm);
		}else
		{
			if( NT_SUCCESS(WingOpenProcess( hNtDll, &hProcess, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,(HANDLE)pe32.th32ProcessID ))){
				LPVOID commandline = NULL;
				if( NT_SUCCESS( WingQueryProcessVariableSize( hNtDll, hProcess, ProcessCommandLineInformation, (PVOID *)&commandline ) ) )
				{	
					process_item->command_line = wing_str_wchar_to_char( (const wchar_t*)((PUNICODE_STRING)commandline)->Buffer ) ;
					free(commandline);
					commandline = NULL;
				}else
				{
					process_item->command_line = new char[4];
					memset( process_item->command_line , 0 , 4 );
				}
			}else
				{
					process_item->command_line = new char[4];
					memset( process_item->command_line , 0 , 4 );
				}
			

		}
		
		
		GetProcessMemoryInfo( handle, &pmc, sizeof(pmc) );
		process_item->working_set_size = pmc.WorkingSetSize;
		

		GetProcessHandleCount( handle ,&process_item->handle_count );
		
		process_item->base_priority = GetPriorityClass( handle );

		CloseHandle( handle );

		
		bMore = ::Process32Next(hProcessSnap, &pe32);
		dwTotalProcess++;
		
		if( dwTotalProcess > max_count ) 
			break;

		
    }  

	if( hProcessSnap ) 
		CloseHandle( hProcessSnap );

	FreeLibrary(hNtDll);

	return dwTotalProcess;
		
}


/**
 *@枚举进程 all_process 参数为null时 只返回进程数量
 *@nt系列函数不会返回当前自身进程信息
 */
unsigned long WingQueryProcess( PROCESSINFO *&all_process , int max_count )
{

	PSYSTEM_PROCESSES			pSystemProc;
	HMODULE						hNtDll         = NULL;
	LPVOID						lpSystemInfo   = NULL;
	DWORD						dwNumberBytes  = MAX_INFO_BUF_LEN;
	DWORD						dwTotalProcess = 0;
	DWORD						dwReturnLength;
	NTSTATUS					Status; 
	LONGLONG					llTempTime;
	NTQUERYSYSTEMINFORMATION	NtQuerySystemInformation;
	
	char *empty_string     = "\0";
	char *sys_process_name = "System Idle Process\0";
	int has_current_process = 0;
	DWORD current_process_id = GetCurrentProcessId();
	
	//__try
	{
		hNtDll = GetModuleHandleA("ntdll.dll");
		if(hNtDll == NULL)
		{
			if(lpSystemInfo != NULL)
			{
				free(lpSystemInfo);
			}
			if(hNtDll != NULL)
			{
				FreeLibrary(hNtDll);
			}
			return 0;
		}
		
		NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress( hNtDll, "NtQuerySystemInformation" );
		if(NtQuerySystemInformation == NULL)
		{
			if(lpSystemInfo != NULL)
			{
				free(lpSystemInfo);
			}
			if(hNtDll != NULL)
			{
				FreeLibrary(hNtDll);
			}
			return 0;
		}

		lpSystemInfo = (LPVOID)malloc(dwNumberBytes);
		Status = NtQuerySystemInformation( SystemProcessInformation, lpSystemInfo, dwNumberBytes, &dwReturnLength);
		if( Status == STATUS_INFO_LENGTH_MISMATCH )
		{
			if(lpSystemInfo != NULL)
			{
				free(lpSystemInfo);
			}
			if(hNtDll != NULL)
			{
				FreeLibrary(hNtDll);
			}
			return 0;
		}
		else if( Status != STATUS_SUCCESS )
		{
			if(lpSystemInfo != NULL)
			{
				free(lpSystemInfo);
			}
			if(hNtDll != NULL)
			{
				FreeLibrary(hNtDll);
			}
			return 0;
		}

		pSystemProc = (PSYSTEM_PROCESSES)lpSystemInfo;

		HANDLE hProcess;

		

		while( pSystemProc->NextEntryDelta != 0 )
		{
			if( all_process == NULL ) {
				dwTotalProcess++;
				pSystemProc = (PSYSTEM_PROCESSES)((char *)pSystemProc + pSystemProc->NextEntryDelta);
				continue;
			}
		
			PROCESSINFO *process_item = &all_process[dwTotalProcess];

			if( pSystemProc->ProcessId != 0 )
			{
				process_item->process_name = wing_str_wchar_to_char( (const wchar_t*)pSystemProc->ProcessName.Buffer ) ;
			}
			else
			{
				
				int len = strlen( sys_process_name );
				process_item->process_name = new char[len+1];
				memset( process_item->process_name , 0 , len+1 );
				sprintf_s( process_item->process_name ,(len+1), "%s" , sys_process_name );
			}

			if( current_process_id == pSystemProc->ProcessId ) {
				has_current_process = 1;
			}


			process_item->command_line = NULL;
			process_item->file_name    = NULL;
			process_item->file_path    = NULL;


			if( NT_SUCCESS(WingOpenProcess( hNtDll, &hProcess, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,(HANDLE)pSystemProc->ProcessId )))
			{
				
				LPVOID commandline = NULL;
				if( NT_SUCCESS( WingQueryProcessVariableSize( hNtDll, hProcess, ProcessCommandLineInformation, (PVOID *)&commandline ) ) )
				{	
					process_item->command_line = wing_str_wchar_to_char( (const wchar_t*)((PUNICODE_STRING)commandline)->Buffer );; 
					free(commandline);
					commandline = NULL;
				}else
				{
					process_item->command_line = new char[4];
					memset( process_item->command_line , 0 , 4 );

				}
			
				PUNICODE_STRING fileName;
				if( NT_SUCCESS( WingQueryProcessVariableSize( hNtDll, hProcess, ProcessImageFileName, (PVOID*)&fileName ))){
					process_item->file_name = wing_str_wchar_to_char( (const wchar_t*)fileName->Buffer );
					free(fileName);
				}
				else
				{
					process_item->file_name    = new char[4];
					memset( process_item->file_name ,    0 , 4 );
		
				}

			
				PUNICODE_STRING filepath;
				if( NT_SUCCESS( WingQueryProcessVariableSize( hNtDll, hProcess, ProcessImageFileNameWin32, (PVOID*)&filepath ))){
					process_item->file_path = wing_str_wchar_to_char( (const wchar_t*)filepath->Buffer );
					free(filepath);
				}
				else
				{
					process_item->file_path    = new char[4];
					memset( process_item->file_path ,    0 , 4 );

				}

			}
			else
			{
				
				process_item->command_line = new char[4];
				process_item->file_name    = new char[4];
				process_item->file_path    = new char[4];

				memset( process_item->command_line , 0 , 4 );
				memset( process_item->file_name ,    0 , 4 );
				memset( process_item->file_path ,    0 , 4 );

			}
			
			process_item->process_id        = pSystemProc->ProcessId;
			process_item->parent_process_id = pSystemProc->InheritedFromProcessId;
			process_item->working_set_size  = pSystemProc->VmCounters.WorkingSetSize;
			process_item->base_priority     = (unsigned long)pSystemProc->BasePriority;
			process_item->thread_count      = pSystemProc->ThreadCount;
			process_item->handle_count      = pSystemProc->HandleCount;


			llTempTime  = pSystemProc->KernelTime.QuadPart + pSystemProc->UserTime.QuadPart;
			llTempTime /= 10000;
			llTempTime /= 1000; //精确到秒

			process_item->cpu_time = (unsigned long)llTempTime;

			dwTotalProcess ++;
			if( dwTotalProcess > (DWORD)max_count ) break;
			pSystemProc = (PSYSTEM_PROCESSES)((char *)pSystemProc + pSystemProc->NextEntryDelta);

			
		}
	}
	//__finally
	{
		if(lpSystemInfo != NULL)
		{
			free(lpSystemInfo);
		}
		if(hNtDll != NULL)
		{
			FreeLibrary(hNtDll);
		}
	}
	
	dwTotalProcess += WingGetCurrentProcessInfo( all_process, dwTotalProcess , max_count - dwTotalProcess , has_current_process);

	return dwTotalProcess;
}

int WingQueryObject( HANDLE handle )
{
	if( handle == INVALID_HANDLE_VALUE || (int)handle == 0) 
	{
		return -1;
	}

	HMODULE	hNtDll = GetModuleHandleA("ntdll.dll");
	if( NULL == hNtDll ) {
		return -1;
	}

	WingNtQueryObject NtQueryObject   = ( WingNtQueryObject )GetProcAddress( hNtDll , "NtQueryObject" );
    SYSTEM_HANDLE_STATE *pOutBuffer   = ( SYSTEM_HANDLE_STATE *)malloc(0x38);
	
	memset(pOutBuffer,0,0x38);

	if( !NT_SUCCESS( NtQueryObject( handle, ObjectBasicInformation, pOutBuffer, 0x38, NULL ) ) )
	{
		free( pOutBuffer );
		return 0;
	}

	int count =  pOutBuffer->HandleCount;

	free( pOutBuffer );
	FreeLibrary(hNtDll);

	return count;
}

int WingWindowsVersion(){
	
	RTL_OSVERSIONINFOEXW versionInfo;
    ULONG majorVersion;
    ULONG minorVersion;

	ULONG WindowsVersion;
	RTL_OSVERSIONINFOEXW PhOsVersion;


    versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	HMODULE	hNtDll = GetModuleHandleA("ntdll.dll");

	if( NULL == hNtDll ) {
		return -1;
	}


	RTLGETVERSION RtlGetVersion = (RTLGETVERSION)GetProcAddress(hNtDll,"RtlGetVersion");
	
    if (!NT_SUCCESS(RtlGetVersion((PRTL_OSVERSIONINFOW)&versionInfo)))
    {
		FreeLibrary(hNtDll);
		return  WING_WINDOWS_NEW ;
    }

    memcpy(&PhOsVersion, &versionInfo, sizeof(RTL_OSVERSIONINFOEXW));
    majorVersion = versionInfo.dwMajorVersion;
    minorVersion = versionInfo.dwMinorVersion;

    if (majorVersion == 5 && minorVersion < 1 || majorVersion < 5)
    {
        WindowsVersion = WING_WINDOWS_ANCIENT;
    }
    /* Windows XP */
    else if (majorVersion == 5 && minorVersion == 1)
    {
        WindowsVersion = WING_WINDOWS_XP;
    }
    /* Windows Server 2003 */
    else if (majorVersion == 5 && minorVersion == 2)
    {
        WindowsVersion = WING_WINDOWS_SERVER_2003;
    }
    /* Windows Vista, Windows Server 2008 */
    else if (majorVersion == 6 && minorVersion == 0)
    {
        WindowsVersion = WING_WINDOWS_VISTA;
    }
    /* Windows 7, Windows Server 2008 R2 */
    else if (majorVersion == 6 && minorVersion == 1)
    {
        WindowsVersion = WING_WINDOWS_7;
    }
    /* Windows 8 */
    else if (majorVersion == 6 && minorVersion == 2)
    {
        WindowsVersion = WING_WINDOWS_8;
    }
    /* Windows 8.1 */
    else if (majorVersion == 6 && minorVersion == 3)
    {
        WindowsVersion = WING_WINDOWS_8_1;
    }
    /* Windows 10 */
    else if (majorVersion == 10 && minorVersion == 0)
    {
        WindowsVersion = WING_WINDOWS_10;
    }
    else if (majorVersion == 10 && minorVersion > 0 || majorVersion > 10)
    {
        WindowsVersion = WING_WINDOWS_NEW;
    }
	FreeLibrary(hNtDll);
	return  WindowsVersion ;
}


unsigned long WingQueryProcessByProcessID(PROCESSINFO *process_item, unsigned long process_id)
{

	PSYSTEM_PROCESSES			pSystemProc;
	HMODULE						hNtDll = NULL;
	LPVOID						lpSystemInfo = NULL;
	DWORD						dwNumberBytes = MAX_INFO_BUF_LEN;
	DWORD						dwTotalProcess = 0;
	DWORD						dwReturnLength;
	NTSTATUS					Status;
	LONGLONG					llTempTime;
	NTQUERYSYSTEMINFORMATION	NtQuerySystemInformation;

	char *empty_string = "\0";
	char *sys_process_name = "System Idle Process\0";
	int has_current_process = 0;
	DWORD current_process_id = GetCurrentProcessId();

	//__try
	{
		hNtDll = GetModuleHandleA("ntdll.dll");
		if (hNtDll == NULL)
		{
			if (lpSystemInfo != NULL)
			{
				free(lpSystemInfo);
			}
			if (hNtDll != NULL)
			{
				FreeLibrary(hNtDll);
			}
			return 0;
		}

		NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(hNtDll, "NtQuerySystemInformation");
		if (NtQuerySystemInformation == NULL)
		{
			if (lpSystemInfo != NULL)
			{
				free(lpSystemInfo);
			}
			if (hNtDll != NULL)
			{
				FreeLibrary(hNtDll);
			}
			return 0;
		}

		lpSystemInfo = (LPVOID)malloc(dwNumberBytes);
		Status = NtQuerySystemInformation(SystemProcessInformation, lpSystemInfo, dwNumberBytes, &dwReturnLength);
		if (Status == STATUS_INFO_LENGTH_MISMATCH)
		{
			if (lpSystemInfo != NULL)
			{
				free(lpSystemInfo);
			}
			if (hNtDll != NULL)
			{
				FreeLibrary(hNtDll);
			}
			return 0;
		}
		else if (Status != STATUS_SUCCESS)
		{
			if (lpSystemInfo != NULL)
			{
				free(lpSystemInfo);
			}
			if (hNtDll != NULL)
			{
				FreeLibrary(hNtDll);
			}
			return 0;
		}

		pSystemProc = (PSYSTEM_PROCESSES)lpSystemInfo;

		HANDLE hProcess;



		while (pSystemProc->NextEntryDelta != 0)
		{
			if (pSystemProc->ProcessId != process_id)
			{
				pSystemProc = (PSYSTEM_PROCESSES)((char *)pSystemProc + pSystemProc->NextEntryDelta);
				continue;
			}
			//PROCESSINFO *process_item = &all_process[dwTotalProcess];

			if (pSystemProc->ProcessId != 0)
			{
				process_item->process_name = wing_str_wchar_to_char((const wchar_t*)pSystemProc->ProcessName.Buffer);
			}
			else
			{

				int len = strlen(sys_process_name);
				process_item->process_name = new char[len + 1];
				memset(process_item->process_name, 0, len + 1);
				sprintf_s(process_item->process_name, (len + 1), "%s", sys_process_name);
			}

			if (current_process_id == pSystemProc->ProcessId) {
				has_current_process = 1;
			}


			process_item->command_line = NULL;
			process_item->file_name = NULL;
			process_item->file_path = NULL;


			if (NT_SUCCESS(WingOpenProcess(hNtDll, &hProcess, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, (HANDLE)pSystemProc->ProcessId)))
			{

				LPVOID commandline = NULL;
				if (NT_SUCCESS(WingQueryProcessVariableSize(hNtDll, hProcess, ProcessCommandLineInformation, (PVOID *)&commandline)))
				{
					process_item->command_line = wing_str_wchar_to_char((const wchar_t*)((PUNICODE_STRING)commandline)->Buffer);;
					free(commandline);
					commandline = NULL;
				}
				else
				{
					process_item->command_line = new char[4];
					memset(process_item->command_line, 0, 4);

				}

				PUNICODE_STRING fileName;
				if (NT_SUCCESS(WingQueryProcessVariableSize(hNtDll, hProcess, ProcessImageFileName, (PVOID*)&fileName))) {
					process_item->file_name = wing_str_wchar_to_char((const wchar_t*)fileName->Buffer);
					free(fileName);
				}
				else
				{
					process_item->file_name = new char[4];
					memset(process_item->file_name, 0, 4);

				}


				PUNICODE_STRING filepath;
				if (NT_SUCCESS(WingQueryProcessVariableSize(hNtDll, hProcess, ProcessImageFileNameWin32, (PVOID*)&filepath))) {
					process_item->file_path = wing_str_wchar_to_char((const wchar_t*)filepath->Buffer);
					free(filepath);
				}
				else
				{
					process_item->file_path = new char[4];
					memset(process_item->file_path, 0, 4);

				}

			}
			else
			{

				process_item->command_line = new char[4];
				process_item->file_name = new char[4];
				process_item->file_path = new char[4];

				memset(process_item->command_line, 0, 4);
				memset(process_item->file_name, 0, 4);
				memset(process_item->file_path, 0, 4);

			}
			//process_item->thread_id = pSystemProc->Threads[0].ClientId;
			process_item->process_id = pSystemProc->ProcessId;
			process_item->parent_process_id = pSystemProc->InheritedFromProcessId;
			process_item->working_set_size = pSystemProc->VmCounters.WorkingSetSize;
			process_item->base_priority = (unsigned long)pSystemProc->BasePriority;
			process_item->thread_count = pSystemProc->ThreadCount;
			process_item->handle_count = pSystemProc->HandleCount;


			llTempTime = pSystemProc->KernelTime.QuadPart + pSystemProc->UserTime.QuadPart;
			llTempTime /= 10000;
			llTempTime /= 1000; //精确到秒

			process_item->cpu_time = (unsigned long)llTempTime;

			dwTotalProcess++;
			//if (dwTotalProcess > (DWORD)max_count) break;
			pSystemProc = (PSYSTEM_PROCESSES)((char *)pSystemProc + pSystemProc->NextEntryDelta);

			break;
		}
	}
	//__finally
	{
		if (lpSystemInfo != NULL)
		{
			free(lpSystemInfo);
		}
		if (hNtDll != NULL)
		{
			FreeLibrary(hNtDll);
		}
	}

	//dwTotalProcess += WingGetCurrentProcessInfo(all_process, dwTotalProcess, max_count - dwTotalProcess, has_current_process);

	return dwTotalProcess;
}