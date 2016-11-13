#ifndef __WING_MALLOC_H__
#define __WING_MALLOC_H__
#include "Windows.h"


void *wing_malloc( int size );
void wing_free( void* mem );

#endif