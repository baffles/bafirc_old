/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** mem.c
 **/

//#include "bafirc.h"
#include "bafirc/mem.h"

#define MALLOC_INTERNAL(s) malloc(s); b__internal_self_mem_usage += s;
#define FREE_INTERNAL(ptr) free(ptr); b__internal_self_mem_usage -= sizeof(*(ptr));

b__internal_memmap_list *b__internal_memmap = NULL;
int b__internal_self_mem_usage = 0;

void *balloc(size_t size)
{
  void *ret;
  b__internal_memmap_list *node = b__internal_memmap;
  if(!node)
  {
    /* we gotta allocate the list \o/ */
    b__internal_memmap = (b__internal_memmap_list *)MALLOC_INTERNAL(sizeof(b__internal_memmap_list));
    if(!b__internal_memmap)
      return NULL;
    node = b__internal_memmap;
  }
  else
  {
    while(node->next)
      node = node->next;
    node->next = (b__internal_memmap_list *)MALLOC_INTERNAL(sizeof(b__internal_memmap_list));
    if(!node->next)
      return NULL;
    node = node->next;
  }
  node->ptr_address = (void *)malloc(size);
  if(!node->ptr_address)
  {
    free(node);
    return NULL;
  }
  //node->size = sizeof(*(node->ptr_address));
  node->size = size;
  node->allocated = 1;
  node->next = NULL;
  
  return node->ptr_address;
}

void *bcalloc(size_t memsize, size_t size)
{
  void *ret = balloc(memsize * size);
  if(!ret)
    return NULL;
  memset(ret, 0, memsize * size);
  return ret;
}

void *brealloc(void *ptr, size_t size)
{
  b__internal_memmap_list *node = b__internal_memmap, *working = NULL;
  if(size == 0)
  {
    bfree(ptr);
    return NULL;
  }
  if(!ptr)
    return balloc(size);
  
  for(; node->next != NULL; node=node->next)
  {
    if(node->ptr_address == ptr)
    {
      working = node;
      break;
    }
  }
  
  if(working == NULL) /* this wasn't found in the list, so just do what they want anyway :P */
    return (void *)realloc((void *)ptr, size);
  
  /* otherwise working == the node for this memory */
  node->ptr_address = (void *)realloc((void *)ptr, size);
  node->allocated = 1;
  node->size = sizeof(*(node->ptr_address));
  
  return node->ptr_address;
}

void bfree(void *ptr)
{
  b__internal_memmap_list *node = b__internal_memmap, *working = NULL, *prev = b__internal_memmap, *tmp;
  if(!ptr)
    return;
  
  while(1)
  {
    if(node->ptr_address == ptr)
    {
      working = node;
      break;
    }
    else if(node->next == NULL)
      break;
    prev = node;
    node = node->next;
  }
  
  if(working == NULL) /* this wasn't found in the list, so just give up :P */
    return;
  node = working;
  
  /* otherwise delete and free it */
  free(node->ptr_address);
  node->size = 0;
  node->allocated = 0;
  tmp = node->next;
  if(node->next == NULL)
  {
    FREE_INTERNAL(node);
    b__internal_memmap = NULL;
  }
  else
  {
    node->next = NULL;
    FREE_INTERNAL(prev->next);
    prev->next = tmp;
  }
}


void print_mem_report(FILE *out)
{
  b__internal_memmap_list *i;
  fprintf(out, "== balloc Memory Usage Report ==\n");
  fprintf(out, " Total Memory Usage: %d bytes\n", mem_usage());
  fprintf(out, "  Internal Memory Usage: %d bytes\n", b__internal_self_mem_usage);
  fprintf(out, "  Program Memory Usage:  %d bytes\n", mem_usage() - b__internal_self_mem_usage);
  fprintf(out, "\n");
  fprintf(out, "Allocation Table:\n");
  if(b__internal_memmap != NULL)
  {
    //for(i = b__internal_memmap; i->next != NULL; i = i->next)
    i = b__internal_memmap;
    while(1)
    {
      fprintf(out, " * Address: %p\tSize: %10d bytes\t\t%s\n", (void *)i, i->size, (i->allocated ? "Is Allocated" : "Isn't Allocated"));
      if(i->next == NULL)
        break;
      i = i->next;
    }
  }
  fprintf(out, "\nEnd of report\n");
}

size_t mem_usage()
{
  if(!b__internal_memmap)
    return 0;
  else
  {
    b__internal_memmap_list *i; size_t tot = 0;
    i = b__internal_memmap;
    while(1)
    {
      if(i->allocated)
        tot += i->size;
      if(!i->next)
        break;
      i = i->next;
    }
    return tot + b__internal_self_mem_usage;
  }
}
