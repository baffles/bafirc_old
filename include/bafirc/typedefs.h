/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** bafirc/callback.h
 **/

#ifndef BAFIRC_TYPEDEFS_H
#define BAFIRC_TYPEDEFS_H

#ifndef BAFIRC_H
#include "bafirc.h"
#endif

/* callback.h */
typedef struct bcallback_info bcallback_info;
typedef int (*bcallback_func)(bcallback_info *);
typedef struct bcallback_list bcallback_list;
  
/* identd.h */
typedef struct bidentd bidentd;
  
/* irc.h */
typedef struct birc_server_info birc_server_info;
typedef struct birc birc;
typedef struct birc_message birc_message;
  
/* mem.h */
typedef struct b__internal_memmap_list b__internal_memmap_list;
  
/* socket.h */
typedef struct bsock bsock;

/* threads.h */
typedef struct bthread bthread;

#endif /* BAFIRC_TYPEDEFS_H */
