/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** identd.c
 **/

#include "bafirc.h"

EXPORT bidentd *bidentd_create(char *id, char *os, birc *irc)
{
#ifdef ON_WINDOWS
  bidentd *ret = (bident *)balloc(sizeof(bidentd));
  if(!ret)
    return NULL;
  ret->thread = bthread_create(bident_thread, ret);
  if(!ret->thread)
  {
    bfree(ret);
    return NULL;
  }
  ret->id = (char *)balloc(strlen(id) + 1);
  if(!ret->id)
  {
    bthread_destroy(ret->thread);
    bfree(ret);
    return NULL;
  }
  strcpy(ret->id, id);
  ret->os = (char *)balloc(strlen(os) + 1);
  if(!ret->os)
  {
    bthread_destroy(ret->thread);
    bfree(ret->id);
    bfree(ret);
    return NULL;
  }
  strcpy(ret->os, os);
  ret->irc = irc;
  return ret;
#endif /* ON_WINDOWS */
}

EXPORT void bidentd_destroy(bident *i)
{
#ifdef ON_WINDOWS
  if(!i)
  {
    bthread_destroy(i->thread);
    bfree(i->id);
    bfree(i->os);
    bfree(i);
  }
#endif /* ON_WINDOWS */
}


EXPORT void bidentd_start(bafirc_identd *i)
{
#ifdef ON_WINDOWS
  bthread_start(i->thread);
#endif /* ON_WINDOWS */
}

EXPORT void *bidentd_thread(void *data)
{
#ifdef ON_WINDOWS
  bthread *self = (btread *)data;
  bidentd *i = (bidentd *)self->data;
  char response[512];
  int ic, s; // sockets
  unsigned int sent;
  char   myname[MAXHOSTNAME+1];
  struct sockaddr_in sa;
  struct hostent *hp;
  
  memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
  gethostname(myname, MAXHOSTNAME);           /* who are we? */
  hp= gethostbyname(myname);                  /* get our address info */
  if(hp == NULL)                             /* we don't exist !? */
    return(NULL);
  sa.sin_family = hp->h_addrtype;              /* this is our host address */
  sa.sin_port = htons(113);                /* this is our port number */
  if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create socket */
    return(NULL);
  if(bind(s, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0)
  {
    close(s);
    return(NULL);
  }
  listen(s, 1);                               /* max # of queued connects */
  ic = accept(s, NULL, NULL);
  
  sprintf(response, "%d , %d : USERID : %s : %s", i->irc->socket->port, ic, i->os, i->id);
  
  // send it all
  sent = 0;
  while(sent < strlen(response))
  {
    sent += send(ic, response + sent, strlen(response) - sent, 0);
  }
  
  close(ic);
  close(s);
#endif /* ON_WINDOWS */
}
