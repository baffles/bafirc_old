/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** bafirc/threads.h
 **/

#ifndef BAFIRC_THREADS_H
#define BAFIRC_THREADS_H

#ifndef BAFIRC_H
#include "bafirc.h"
#endif

struct bthread
{
/*#ifdef ON_WINDOWS
  HANDLE thread;
  DWORD thread_id;
#else*/
  pthread_t thread;
//#endif
  thread_func tfunc;
  void *data;
  BOOL die;
};

EXPORT bthread *bthread_create(thread_func tfunc, void *data);
EXPORT void bthread_start(bthread *t);
EXPORT void bthread_stop(bthread *t);
EXPORT void bthread_destroy(bthread *t);

#endif /* BAFIRC_THREADS_H */
