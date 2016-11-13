#ifndef __WING_QUERY_PROCESS__
#define __WING_QUERY_PROCESS__

#define WING_ERROR_NT -1

#define  WING_WINDOWS_ANCIENT           0
#define  WING_WINDOWS_XP                51
#define  WING_WINDOWS_SERVER_2003       52
#define  WING_WINDOWS_VISTA             60
#define  WING_WINDOWS_7                 61
#define  WING_WINDOWS_8                 62
#define  WING_WINDOWS_8_1               63
#define  WING_WINDOWS_10                100
#define  WING_WINDOWS_NEW               MAXLONG

typedef struct _PROCESSINFO {
	char *process_name;
	char *command_line;
	char *file_name;
	char *file_path;
	int process_id;
	//int thread_id;
	int parent_process_id;
	unsigned long working_set_size;
	unsigned long base_priority;//基本的优先级
	unsigned long thread_count ;
	unsigned long handle_count ;
	unsigned long cpu_time;
} PROCESSINFO;

unsigned long  WingQueryProcess( PROCESSINFO *&all_process , int max_count );
unsigned long WingQueryProcessByProcessID(PROCESSINFO *process_item, unsigned long process_id);
int WingQueryObject( HANDLE handle );
int WingWindowsVersion();

#endif