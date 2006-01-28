/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** threads.c
 **/

#include "bafirc.h"
#define balloc malloc
#define bfree free
#define brealloc realloc
#define bcalloc calloc
#define print_mem_report(a)
#define print_mem_report_irc(a, b)

bthread *bthread_create(thread_func tfunc, void *data)
{
  bthread *ret;
  
  ret = (bthread *)balloc(sizeof(bthread));
  if(ret == NULL)
    return NULL;
  
  ret->tfunc = tfunc;
  ret->data = data;
  
  return ret;
}

void bthread_start(bthread *t)
{
  t->die = 0;
//#ifdef ON_WINDOWS
//  t->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)t->tfunc, (LPVOID)t->data, 0, &t->thread_id);
//#else
  pthread_create(&t->thread, NULL, t->tfunc, (void *)t);
  pthread_detach(t->thread);
//#endif
}

void bhtread_stop(bthread *t)
{
  t->die = TRUE;
}

void bthread_destroy(bthread *t)
{
/*#ifdef ON_WINDOWS
  CloseHandle(t->thread);
#endif*/
  bfree(t->data);
  bfree(t);
  t = NULL;
}
