/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** bafirc/irc.h
 **/

#ifndef BAFIRC_IRC_H
#define BAFIRC_IRC_H

#ifndef BAFIRC_H
#include "bafirc.h"
#endif

typedef struct birc_server_info
{
  char *server;
  int port;
  
  char motd[10240]; int motd_i;
} birc_server_info;

typedef struct birc
{
  birc_server_info *info;
  bthread *thread;
  bidentd *identd;
  struct bsock *socket;
  unsigned char use_identd;
  int lag, last_ping;
  char *server;
  char **nicks;
  char *username;
  char *password;
  char *realname;
  bcallback_list *callbacks;
} birc;

typedef struct birc_message
{
  int num_params;
  char *message;
  char *nickname;
  char *username;
  char *hostname;
  char *command;
  char **params;
} birc_message;

EXPORT birc *birc_connect(char *server, int port, char *nick1, char *nick2, char *nick3, char *username, char *realname, char *password, unsigned char use_identd);
EXPORT void birc_disconnect(birc *i, char *reason);
EXPORT void birc_destroy(birc *i);

EXPORT void *birc_thread(void *data);

EXPORT birc_message *birc_parse(char *msg);
EXPORT int birc__add_param(birc_message *m, const char *param);
EXPORT void birc_destroy_message(birc_message *m);

#endif /* BAFIRC_IRC_H */
