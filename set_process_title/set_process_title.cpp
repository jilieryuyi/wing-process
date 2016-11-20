// set_process_title.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Windows.h"

//Fypher
//http://hi.baidu.com/nmn714
/* 
VOID ChangeName(ULONG pProcess) {

	ULONG peb, ProcessParameters, ldr;

	ULONG InLoadOrderModuleList;

	ULONG InMemoryOrderModuleList;

	ULONG tmp;

	KAPC_STATE kapc;

	PUCHAR str;

	PWCHAR wstr;



	//get PEB

	peb = *(PULONG)(pProcess + 0x1b0);



	KeStackAttachProcess((PEPROCESS)pProcess, &kapc);

	__try {

		ProcessParameters = *(PULONG)(peb + 0x010);

		//ImagePathName

		FindAndChangeUni(ProcessParameters + 0x038);

		//CommandLine

		FindAndChangeUni(ProcessParameters + 0x040);

		//WindowTitle

		FindAndChangeUni(ProcessParameters + 0x070);

		//Ldr

		ldr = *(PULONG)(peb + 0x00c);

		//InLoadOrderModuleList->FullDllName

		InLoadOrderModuleList = *(PULONG)(ldr + 0x00c);

		FindAndChangeUni(InLoadOrderModuleList + 0x024);

		//InLoadOrderModuleList->BaseDllName

		FindAndChangeUni(InLoadOrderModuleList + 0x02c);

		//InMemoryOrderModuleList->FullDllName

		InMemoryOrderModuleList = *(PULONG)(ldr + 0x014);

		FindAndChangeUni(InMemoryOrderModuleList + 0x024);

	}
	__except (1) {

		KdPrint(("exception occured!"));

	}

	KeUnstackDetachProcess(&kapc);

	//EPROCESS-->ImageFileName

	FindAndChangeA(pProcess + 0x174, 16);

	//EPROCESS-->SeAuditProcessCreationInfo->ImageFileName

	FindAndChangeUni(*(PULONG)(pProcess + 0x1F4));

	//EPROCESS->SectionObject->Segment->ControlArea->FileObject->FileName

	//should use MmIsAddressValid to verify

	tmp = *(PULONG)(pProcess + 0x138);

	tmp = *(PULONG)(tmp + 0x14);

	tmp = *(PULONG)tmp;

	tmp = *(PULONG)(tmp + 0x024);

	FindAndChangeUni(tmp + 0x030);



	//VAD

	//should use MmIsAddressValid to verify

	tmp = *(PULONG)(pProcess + 0x11c);

	tmp = *(PULONG)(tmp + 0x10);

	tmp = *(PULONG)(tmp + 0x018);

	tmp = *(PULONG)(tmp + 0x024);

	FindAndChangeUni(tmp + 0x030);

}



//其中，FindAndChangeUni和FindAndChangeA的作用是在一个字符串（UNICODE_STRING或CHAR）中定位“winmine.exe”并改成"winxxoo.exe"。代码如下:
//Fypher

//http://hi.baidu.com/nmn714

VOID FindAndChangeUni(ULONG strAddr) {

	PUNICODE_STRING uniStr = (PUNICODE_STRING)strAddr;

	ULONG len = uniStr->Length / 2;

	ULONG maxLen = uniStr->MaximumLength / 2;

	PWCHAR str = uniStr->Buffer;

	ULONG i = 0;

	if (!str || len<11 || maxLen<11)

		return;

	for (i = 0;i <= len - 11;++i) {

		if (!_wcsnicmp(str + i, L"winmine.exe", 11))

			break;

	}

	if (i>len - 11)

		return;



	_asm {

		cli

		mov eax, cr0

		and eax, not 0x10000

		mov cr0, eax

	}

	//str可能是PEB中的，故try之

	__try {

		str[i + 3] = L'x';

		str[i + 4] = L'x';

		str[i + 5] = L'o';

		str[i + 6] = L'o';

	}
	__except (1) {

	}

	_asm {

		mov eax, cr0

		or eax, 0x10000

		mov cr0, eax

		sti

	}

}

VOID FindAndChangeA(ULONG strAddr, ULONG len) {

	PUCHAR str = (PUCHAR)strAddr;

	ULONG i = 0;

	if (!str || len<11)

		return;

	for (i = 0;i <= len - 11;++i) {

		if (!_strnicmp(str + i, "winmine.exe", 11))

			break;

	}

	if (i>len - 11)

		return;



	_asm {

		cli

		mov eax, cr0

		and eax, not 0x10000

		mov cr0, eax

	}

	//str可能是PEB中的，故try之

	__try {

		str[i + 3] = 'x';

		str[i + 4] = 'x';

		str[i + 5] = 'o';

		str[i + 6] = 'o';

	}
	__except (1) {

	}



	_asm {

		mov eax, cr0

		or eax, 0x10000

		mov cr0, eax

		sti

	}

}

//目前Ark所用的获取进程路径的方法：PEB，
//EPROCESS ->ImageFileName，
//EPROCESS ->SeAuditProcessCreationInfo，
//EPROCESS ->SectionObject->SegmentObject->FileObject->FilePath，
//EPROCESS ->VadRoot等方法。然后逐一抹掉。。
//关键代码：
//OnInit中获得RtlInitUnicodeString的地址用于初始化字符串
void OnInit() {
	RtlInitUnicodeString = (pfnRtlInitUnicodeString)GetProcAddress(
		GetModuleHandleW(L"ntdll.dll"),
		"RtlInitUnicodeString"
	);
	//初始化字符串。
	RtlInitUnicodeString(&ustrFullName, L"C:\\Windows\\System32\\svchost.exe");
	RtlInitUnicodeString(&ustrBaseName, L"svchost.exe");
	//////////////////////
	PPEB_LDR_DATA pLdr = NULL;
	PLDR_DATA_TABLE_ENTRY pLdt = NULL;
	PLIST_ENTRY pList, pHead;
	DWORD dwProcessParameters = 0;
	__asm
	{
		mov eax, fs:[0x30]
		mov ecx, [eax + 0x0c] //Ldr
		mov pLdr, ecx
		mov ecx, [eax + 0x10]
		mov dwProcessParameters, ecx
	}
	dprintf(L"dwProcessParameters : 0x%08X.", dwProcessParameters);
	dprintf(L"pLdr : 0x%08X", pLdr);
	*(UNICODE_STRING *)(dwProcessParameters + 0x038) = ustrFullName;//ImagePathName
	*(UNICODE_STRING *)(dwProcessParameters + 0x040) = ustrFullName;// CommandLine
	*(UNICODE_STRING *)(dwProcessParameters + 0x070) = ustrFullName;//
	pHead = pLdr - &gt;InLoadOrderModuleList.Flink;
	//遍历模块链表，找到Exe。
	pList = pHead;
	do
	{
		pLdt = CONTAINING_RECORD(
			pList,
			LDR_DATA_TABLE_ENTRY,
			InLoadOrderLinks);
		if (pLdt - &gt;DllBase)
		{
			if (!wcscmp(pLdt - &gt;BaseDllName.Buffer, L"fakeExe.exe"))
			{
				dprintf(L"Module : %s", pLdt - &gt;FullDllName.Buffer);
				pLdt - &gt;FullDllName = ustrFullName;//修改指针
				pLdt - &gt;BaseDllName = ustrBaseName;//同上
			}
		}
		pList = pList - &gt;Flink;
	} while (pList != pHead);

}
NTSTATUS DispatchCreate(
	PDEVICE_OBJECT pDevObj,
	PIRP pIrp
)
{
	PEPROCESS currProcess;
	currProcess = IoGetCurrentProcess();//获得当前进程的EPROCESS
	SvchostProcess = GetSvchostProcess();//通过活动进程链找一个svchost.exe
	dprintf("CurrProcess : 0x%08X.\n", currProcess);
	fakeProcess(currProcess);//开始伪造
	pIrp - &gt;IoStatus.Status = STATUS_SUCCESS;
	pIrp - &gt;IoStatus.Information = 0;
	dprintf("[ke_fake_exe] DispatchCreate\n");
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
VOID fakeProcess(
	PEPROCESS Process
)
{
	PUCHAR pImageName = NULL;
	PCONTROL_AREA pSvchostCa = NULL;
	PCONTROL_AREA pCurrCa = NULL;
	pImageName = PsGetProcessImageFileName(Process);//修改EPROCESS-&gt;ImageFileName
	if (pImageName)
	{
		strcpy(pImageName, "svchost.exe");
	}
	*(ULONG*)((ULONG)Process + 0x1f4) = *(ULONG*)((ULONG)SvchostProcess + 0x1f4);
	//修改EPROCESS-&gt;SeAuditProcessCreationInfo
	pSvchostCa = GetProcessControlAera(SvchostProcess);
	//得到正常svchost.exe的ControlAera
	pCurrCa = GetProcessControlAera(Process);
	//本进程的ControlAera
	pCurrCa - &gt;FilePointer = pSvchostCa - &gt;FilePointer;
	//修改
	//由于VAD里指向ControlAera指向同一个地方，所以修改一处即可。
}
PEPROCESS GetSvchostProcess(
)
//通过活动进程链枚举进程，代码略。
//获取ControlAera。
PCONTROL_AREA GetProcessControlAera(
	PEPROCESS Process
)
{
	PCONTROL_AREA pControlAera = NULL;
	__asm
	{
		mov eax, Process
		mov eax, [eax + 0x138]//SectionObject
		mov eax, [eax + 0x014]//SegmentObject
		mov eax, [eax]
		mov pControlAera, eax
	}
	return pControlAera;
}*/
#include <windows.h>
#include <stdio.h>
#include <aclapi.h>
#include <conio.h>

/*
* you'll find a list of NTSTATUS status codes in the DDK header
* ntstatus.h (\WINDDK\2600.1106\inc\ddk\wxp\)
*/
#define NT_SUCCESS(status)          ((NTSTATUS)(status)>=0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED        ((NTSTATUS)0xC0000022L)

/*
*************************************************************************
* ntddk.h
*/
typedef LONG    NTSTATUS;
typedef ULONG    ACCESS_MASK;
/*
* ntdef.h
*************************************************************************
*/

/*
*************************************************************************
* <<Windows NT/2000 Native API Reference>> - Gary Nebbett
*/

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS;

/*
*Information Class 16
*/
typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG            ProcessId;
	UCHAR            ObjectTypeNumber;
	UCHAR            Flags;
	USHORT            Handle;
	PVOID            Object;
	ACCESS_MASK        GrantedAccess;
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

#define InitializeObjectAttributes( p, n, a, r, s ) { (p)->Length = sizeof( OBJECT_ATTRIBUTES ); (p)->RootDirectory = r; (p)->Attributes = a; (p)->ObjectName = n; (p)->SecurityDescriptor = s; (p)->SecurityQualityOfService = NULL; }
/*
*************************************************************************
* <<Windows NT/2000 Native API Reference>> - Gary Nebbett
*************************************************************************
*/
typedef ULONG(__stdcall *RTLNTSTATUSTODOSERROR) (IN NTSTATUS Status);
typedef NTSTATUS(__stdcall *ZWQUERYSYSTEMINFORMATION) (IN SYSTEM_INFORMATION_CLASS SystemInformationClass, IN OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength OPTIONAL);
/************************************************************************
*                                                                      *
*                            Function Prototype                        *
*                                                                      *
************************************************************************/

static DWORD GetEprocessFromPid(ULONG PID);
static BOOL  LocateNtdllEntry(void);


/************************************************************************
*                                                                      *
*                            Static Global Var                         *
*                                                                      *
************************************************************************/

static RTLNTSTATUSTODOSERROR    RtlNtStatusToDosError = NULL;
static ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = NULL;

static HMODULE                    hModule = NULL;
/************************************************************************/


static DWORD GetEprocessFromPid(ULONG    PID)
{
	NTSTATUS                    status;
	PVOID                        buf = NULL;
	ULONG                        size = 1;
	ULONG                        NumOfHandle = 0;
	ULONG                        i;
	PSYSTEM_HANDLE_INFORMATION    h_info = NULL;

	for (size = 1; ; size *= 2)
	{
		if (NULL == (buf = calloc(size, 1)))
		{
			fprintf(stderr, "calloc( %u, 1 ) failed\n", size);
			goto GetEprocessFromPid_exit;
		}
		status = ZwQuerySystemInformation(SystemHandleInformation, buf, size, NULL);
		if (!NT_SUCCESS(status))
		{
			if (STATUS_INFO_LENGTH_MISMATCH == status)
			{
				free(buf);
				buf = NULL;
			}
			else
			{
				printf("ZwQuerySystemInformation() failed");
				goto GetEprocessFromPid_exit;
			}
		}
		else
		{
			break;
		}
	}  /* end of for */

	   //返回到缓冲区的首先是一个ULONG类型的数据,表示有多少数组
	NumOfHandle = (ULONG)buf;

	h_info = (PSYSTEM_HANDLE_INFORMATION)((ULONG)buf + 4);

	for (i = 0; i<NumOfHandle;i++)
	{
		if ((h_info[i].ProcessId == PID) && (h_info[i].ObjectTypeNumber == 5))//&&( h_info[i].Handle==0x3d8 ) )
		{
			printf("Handle:0x%x,OBJECT 0x%x\n\r", h_info[i].Handle, h_info[i].Object);
			return((DWORD)(h_info[i].Object));
		}
	}
GetEprocessFromPid_exit:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	return(FALSE);
}


/*
* ntdll.dll
*/
static BOOL LocateNtdllEntry(void)
{
	BOOL    ret = FALSE;
	char    NTDLL_DLL[] = "ntdll.dll";
	HMODULE ntdll_dll = NULL;


	if ((ntdll_dll = GetModuleHandle(NTDLL_DLL)) == NULL)
	{
		printf("GetModuleHandle() failed");
		return(FALSE);
	}
	if (!(ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(ntdll_dll, "ZwQuerySystemInformation")))
	{
		goto LocateNtdllEntry_exit;
	}
	ret = TRUE;

LocateNtdllEntry_exit:

	if (FALSE == ret)
	{
		printf("GetProcAddress() failed");
	}
	ntdll_dll = NULL;
	return(ret);
}  /* end of LocateNtdllEntry */


int main(int argc, char **argv)
{

	LocateNtdllEntry();

	//打开自身句柄,这样才能在handle列表中找到自己,PROCESS 对应 ObjectTypeNum 为5
	OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());

	DWORD Addr = GetEprocessFromPid((DWORD)GetCurrentProcessId());

	printf("result: Current EPROCESS's Address is 0x%x \n\r", Addr);

	return TRUE;
}

