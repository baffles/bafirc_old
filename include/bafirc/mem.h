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

struct b__internal_memmap_list
{
  void *ptr_address;
  size_t size;
  unsigned char allocated;
  struct b__internal_memmap_list *next;
};

EXPORT void *balloc(size_t size);
EXPORT void *bcalloc(size_t memsize, size_t size);
EXPORT void *brealloc(void *ptr, size_t size);
EXPORT void bfree(void *ptr);

EXPORT void print_mem_report(FILE *out);
EXPORT void print_mem_report_irc(birc *irc, char *chan);
EXPORT size_t mem_usage();

#endif
