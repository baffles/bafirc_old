/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** bafirc/identd.h
 **/

#ifndef BAFIRC_IDENTD_H
#define BAFIRC_IDENTD_H

#ifndef BAFIRC_H
#include "bafirc.h"
#endif

typedef struct bidentd
{
  bthread *thread;
  char *id, *os;
  struct birc *irc;
} bidentd;

EXPORT bidentd *bidentd_create(char *id, char *os, struct birc *irc);
EXPORT void bidentd_destroy(bidentd *i);

EXPORT void bidentd_start(bidentd *i);
EXPORT void *bidentd_thread(void *data);

#endif /* BAFIRC_IDENTD_H */
