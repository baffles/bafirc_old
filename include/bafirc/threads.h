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

//typedef void (*thread_func)(void *, bafirc_thread *);// thread_func;
typedef void * (*thread_func)(void *);

struct bthread
{
  pthread_t thread;
  thread_func tfunc;
  //void *(*tfunc) (void *);
  void *data;
  BOOL die;
};

EXPORT bthread *bthread_create(thread_func tfunc, void *data);
EXPORT void bthread_start(bthread *t);
EXPORT void bthread_stop(bthread *t);
EXPORT void bthread_destroy(bthread *t);

#endif /* BAFIRC_THREADS_H */
