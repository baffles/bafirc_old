/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** mem.c
 **/

#include "bafirc.h"

b__internal_memmap_list *b__internal_memmap = NULL;
int b__internal_self_mem_usage = 0;

/* internal functions */
b__internal_memmap_list *balloc_node()
{
  void *ret = NULL;
  ret = (b__internal_memmap_list *)malloc(sizeof(b__internal_memmap_list));
  if(!ret)
    return NULL;
  b__internal_self_mem_usage += sizeof(b__internal_memmap_list);
  return ret;
}
void bfree_node(void *ptr)
{
  if(!ptr)
    return;
  free(ptr);
  b__internal_self_mem_usage -= sizeof(b__internal_memmap_list);
}

void *balloc(size_t size)
{
  void *ret;
  b__internal_memmap_list *node = b__internal_memmap;
  if(!node)
  {
    /* we gotta allocate the list \o/ */
    b__internal_memmap = balloc_node();
    if(!b__internal_memmap)
      return NULL;
    node = b__internal_memmap;
  }
  else
  {
    while(node->next)
      node = node->next;
    node->next = balloc_node();
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
  printf("ptr is 0x%p -- size is %d\n", ptr, size);
  b__internal_memmap_list *node = b__internal_memmap, *working = NULL;
  if(size == 0)
  {
    bfree(ptr);
    return NULL;
  }
  if(!ptr)
    return balloc(size);
  
  while(1)
  {
    if(node->ptr_address == ptr)
    {
      working = node;
      break;
    }
    if(!node->next)
      break;
    node = node->next;
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
    printf("bfree: checking %p\n", ptr);
    if(node == NULL)
      break;
    if(node->ptr_address == ptr)
    {
      working = node;
      break;
    }
    if(node->next == NULL)
      break;
    prev = node;
    node = node->next;
  }
  
  if(working == NULL) /* this wasn't found in the list, so just give up :P */
    return;
  node = working;
  
  printf("bfree: freeing %p\n", ptr);
  
  /* otherwise delete and free it */
  free(node->ptr_address);
  node->size = 0;
  node->allocated = 0;
  tmp = node->next;
  if(node->next == NULL)
  {
    bfree_node(node);
    b__internal_memmap = NULL;
  }
  else if(node == prev)
  {
    bfree_node(node);
    b__internal_memmap = tmp;
  }
  else
  {
    node->next = NULL;
    bfree_node(prev->next);
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
      fprintf(out, " * Address: 0x%p\tSize: %10d bytes\t\t%s\n", (void *)i, i->size, (i->allocated ? "Is Allocated" : "Isn't Allocated"));
      if(i->next == NULL)
        break;
      i = i->next;
    }
  }
  fprintf(out, "\nEnd of report\n");
}

void print_mem_report_irc(birc *irc, char *chan)
{
  b__internal_memmap_list *i;
  bsock_send_fmt(irc->socket, "PRIVMSG %s :== balloc Memory Usage Report ==\r\n", chan);
  Sleep(1000);
  bsock_send_fmt(irc->socket, "PRIVMSG %s : Total Memory Usage: %d bytes\r\n", chan, mem_usage());
  Sleep(1000);
  bsock_send_fmt(irc->socket, "PRIVMSG %s :  Internal Memory Usage: %d bytes\r\n", chan, b__internal_self_mem_usage);
  Sleep(1000);
  bsock_send_fmt(irc->socket, "PRIVMSG %s :  Program Memory Usage:  %d bytes\r\n", chan, mem_usage() - b__internal_self_mem_usage);
  Sleep(1000);
  bsock_send_fmt(irc->socket, "PRIVMSG %s : \r\n", chan);
  Sleep(1000);
  bsock_send_fmt(irc->socket, "PRIVMSG %s :Allocation Table:\r\n", chan);
  Sleep(1000);
  if(b__internal_memmap != NULL)
  {
    //for(i = b__internal_memmap; i->next != NULL; i = i->next)
    i = b__internal_memmap;
    while(1)
    {
      bsock_send_fmt(irc->socket, "PRIVMSG %s : * Address: 0x%p\tSize: %10d bytes\t\t%s\r\n", chan, (void *)i, i->size, (i->allocated ? "Is Allocated" : "Isn't Allocated"));
      Sleep(2000);
      if(i->next == NULL)
        break;
      i = i->next;
    }
  }
  bsock_send_fmt(irc->socket, "PRIVMSG %s :End of report\r\n", chan);
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
