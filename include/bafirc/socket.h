/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** socket.h
 **/

#ifndef BAFIRC_SOCKET_H
#define BAFIRC_SOCKET_H

#ifndef BAFIRC_H
#include "bafirc.h"
#endif

typedef struct bsock
{
  struct sockaddr_in destination;
  char *hostname;
  int port;
  int socket;
  unsigned char connected;
  unsigned char non_blocking;
} bsock;

EXPORT int bsock_send(bsock *s, char *buf, int len);
EXPORT int bsock_send_fmt(bsock *s, char *fmt, ...);
EXPORT int bsock_recv(bsock *s, char *buf, int len);

EXPORT bsock *bsock_connect(char *host, int port, unsigned char non_blocking);
EXPORT void bsock_disconnect(bsock *s);
EXPORT void bsock_destroy(bsock *s);

EXPORT int bsock_init();
EXPORT void bsock_deinit();

#endif
