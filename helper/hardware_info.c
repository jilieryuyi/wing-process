#include "hardware_info.h"
#include "WingString.class.h"
#include "WingWmic.class.h"
#include <Iphlpapi.h>  
#include <ctype.h>  
#include <comdef.h>
#include <Wbemidl.h>


#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Iphlpapi")  
#pragma comment(lib, "ws2_32")  

void get_cpu_id( char *&processor_id ){
	
	char *sql = "SELECT * FROM Win32_Processor";

	WingWmic mic;
	
	mic.query(sql);
	char *processor_id_item = NULL;
	WingString ws_processor_id;

	while( mic.next() ) {
		processor_id_item = mic.get("ProcessorId");
		if( processor_id_item != NULL )
		{
			ws_processor_id.append(processor_id_item);
			free( processor_id_item );
			processor_id_item = NULL;
		}
	}
 
	processor_id = ws_processor_id.c_str();
}

/**
 * @ Ó²ÅÌµÄÐòÁÐºÅ
 */
void get_serial_number( char *&serial_number )
{
	char *sql = "SELECT * FROM Win32_PhysicalMedia";
    WingWmic mic;
	
	mic.query(sql);
	char *serial_number_item = NULL;
	WingString ws_serial_number;

	while( mic.next() ) {
		serial_number_item = mic.get("SerialNumber");
		if( serial_number_item != NULL )
		{	
			ws_serial_number.append(serial_number_item);
			free( serial_number_item );
			serial_number_item = NULL;
		}
	}
 
	serial_number = ws_serial_number.c_str();
}