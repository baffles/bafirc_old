/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** mem.h
 **/

#ifndef BAFIRC_MEM_H
#define BAFIRC_MEM_H

#ifndef BAFIRC_H
  //#include "bafirc.h"
#endif

#include <string.h>
#include <stdio.h>

typedef struct b__internal_memmap_list
{
 void *ptr_address;
 size_t size;
 unsigned char allocated;
 struct b__internal_memmap_list *next;
} b__internal_memmap_list;

void *balloc(size_t size);
void *bcalloc(size_t memsize, size_t size);
void *brealloc(void *ptr, size_t size);
void bfree(void *ptr);

void print_mem_report(FILE *out);
size_t mem_usage();

#endif
