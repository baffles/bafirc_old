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

/* Function: bafirc_create_thread
 * Arguments: tfunc - thread function
 *            data  - data to be passed to the thread upon start
 * Description: Makes a thread data structure, preparing it so you can
 *              start it with bafirc_thread_start
 * Return: pointer to the data for the thread
 */
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

/* Function: bafirc_thread_start
 * Arguments: t - the thread to start
 * Description: starts thread `t`
 * Return: nothin'
 */
void bthread_start(bthread *t)
{
  t->die = 0;
  pthread_create(&t->thread, NULL, t->tfunc, (void *)t);
  pthread_detach(t->thread);
}

/* Function: bafirc_thread_stop
 * Arguments: t - the thread to stop 
 * Description: stops thread `t`
 * Return: nothin'
 */
void bhtread_stop(bthread *t)
{
  t->die = TRUE;
}

/* Function: bafirc_destroy_thread
 * Arguments: t - the thread to free from memory
 * Description: frees `t` from memory
 * Return: nothin'
 */
void bthread_destroy(bthread *t)
{
  bfree(t->data);
  bfree(t);
  t = NULL;
}
