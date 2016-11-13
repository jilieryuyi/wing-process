
#include "Windows.h"
#include "synchapi.h"
#include "time.h"
#include <iostream>
#include <hash_map>

using namespace std;
hash_map<unsigned long,unsigned long> select_sockets_hash_map;

void select_add_to_map( unsigned long socket,unsigned long ovl ){
	if( socket <=0 || ovl<=0 ) return;
	select_sockets_hash_map[socket] = ovl;
}

unsigned long select_get_form_map( unsigned long socket ){
	if( select_sockets_hash_map.find(socket) == select_sockets_hash_map.end() ) return 0;
	return select_sockets_hash_map[socket];
}

void select_remove_form_map( unsigned long socket ){
	if( socket <=0 ) return;
	select_sockets_hash_map.erase(socket);
}

unsigned int select_map_size(){
	return select_sockets_hash_map.size();
}

hash_map<unsigned long,unsigned long> select_map(){
	return select_sockets_hash_map;
}

