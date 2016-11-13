#ifndef _IOCP_SOCKET_MAP_H
#define _IOCP_SOCKET_MAP_H
#include "Windows.h"
#include "synchapi.h"
#include "time.h"
#include <iostream>
#include <hash_map>


void iocp_add_to_map( unsigned long socket,unsigned long ovl );
unsigned long iocp_get_form_map( unsigned long socket );
void iocp_remove_form_map( unsigned long socket );
unsigned int iocp_map_size();
//hash_map<unsigned long,unsigned long> iocp_map();

#endif
