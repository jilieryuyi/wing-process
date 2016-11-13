
#include "wing_iocp_socket_map.h"

using namespace std;
hash_map<unsigned long,unsigned long> iocp_sockets_hash_map;

void iocp_add_to_map( unsigned long socket,unsigned long ovl ){
	if( socket <=0 || ovl<=0 ) return;
	iocp_sockets_hash_map[socket] = ovl;
}

unsigned long iocp_get_form_map( unsigned long socket ){
	if( iocp_sockets_hash_map.find(socket) == iocp_sockets_hash_map.end() ) return 0;
	return iocp_sockets_hash_map[socket];
}

void iocp_remove_form_map( unsigned long socket ){
	if( socket <=0 ) return;
	iocp_sockets_hash_map.erase(socket);
}

unsigned int iocp_map_size(){
	return iocp_sockets_hash_map.size();
}

hash_map<unsigned long,unsigned long> iocp_map(){
	return iocp_sockets_hash_map;
}

