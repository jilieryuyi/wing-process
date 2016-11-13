#include "wing_malloc.h"

void *wing_malloc( int size ){
	return malloc( size );
}
void wing_free( void* mem ){
	free( mem );
	mem = NULL;
}