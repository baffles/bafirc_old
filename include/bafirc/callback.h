/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** bafirc/callback.h
 **/

#ifndef BAFIRC_CALLBACK_H
#define BAFIRC_CALLBACK_H

#ifndef BAFIRC_H
#include "bafirc.h"
#endif

typedef struct bcallback_info
{
  struct birc_message *message;
  struct birc *irc;
} bcallback_info;

typedef int (*bcallback_func)(bcallback_info *);

typedef struct bcallback_list
{
  bcallback_func function;
  char *trigger;
  struct bcallback_list *next;
} bcallback_list;

extern bcallback_list *bcallback_global_list;

EXPORT int bcallback_global_install(bcallback_func func, char *trigger);
EXPORT int bcallback_global_uninstall(bcallback_func func);

EXPORT int bcallback_install(struct birc *irc, bcallback_func func, char *trigger);
EXPORT int bcallback_uninstall(struct birc *irc, bcallback_func func);

EXPORT int bcallback_handle(struct birc_message *m, struct birc *irc);

EXPORT int bcallback_init();
EXPORT void bcallback_deinit();

#endif /* BAFIRC_CALLBACK_H */
