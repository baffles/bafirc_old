/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** socket.c
 **/

//#include "bafirc.h"
#include "bafirc/socket.h"

#define MALLOC_INTERNAL(s) malloc(s); b__internal_self_mem_usage += s;
#define FREE_INTERNAL(ptr) free(ptr); b__internal_self_mem_usage -= sizeof(*(ptr));


