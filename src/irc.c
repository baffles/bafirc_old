/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** irc.c
 **/

#include "bafirc.h"

birc *birc_connect(char *server, int port, char *nick1, char *nick2, char *nick3, char *username,
  char *realname, char *password, unsigned char use_identd)
{
  birc *ret;
  char connect_cmd[1024], our_host[512];
  int nicklen[3];
  
  /* get our hostname */
  LOG("Attempting to look up our hostname...");
  if(gethostname(&our_host, 512) == -1)
  {
    LOG("Couldn't look up our hostname, setting it to localhost");
    sprintf(our_host, "localhost");
  }
  
  /* get our nick lengths */
  nicklen[0] = strlen(nick1) + 1;
  nicklen[1] = strlen(nick2) + 1;
  nicklen[2] = strlen(nick3) + 1;
  
  /* do the connection */
  ret = (birc *)balloc(sizeof(birc));
  
  /* make the server info */
  ret->info = (birc_server_info *)balloc(sizeof(birc_server_info));
  ret->info->server = (char *)balloc(strlen(server));
  strcpy(ret->info->server, server);
  ret->info->port = port;
  ret->info->motd_i = 0;
  
  ret->thread =  bthread_create(birc_thread, ret);
#ifdef ON_WINDOWS
  if(use_identd)
  {
    LOG("Starting up IdentD.");
    ret->identd = bidentd_create(username, "UNIX");
    bidentd_start(ret->identd);
  }
  else
    ret->identd = NULL;
#endif
  LOG("Trying to connect...");
  ret->socket = bsock_connect(server, port, 1);
  if(!ret ->socket|| !ret->socket->connected)
  {
    LOG("connect failed. panicking.");
    bidentd_destroy(ret->identd);
    bfree(ret->info->server);
    bfree(ret->info);
    bfree(ret);
    ret = NULL;
    return NULL;
  }
  LOG("Connect Successful.");
#ifdef ON_WINDOWS
    ret->use_identd = use_identd;
#else
    ret->use_identd = 0;
#endif /* ON_WINDOWS */
  ret->lag = 0;
  ret->server = (char *)balloc((strlen(server) + 1));
  strcpy(ret->server, server);

  ret->nicks[0] = (char *)balloc(nicklen[0]);
  if(!ret->nicks[0])
  {
    bidentd_destroy(ret->identd);
    bfree(ret->info->server);
    bfree(ret->info);
    bfree(ret->server);
    bfree(ret);
    ret = NULL;
    return NULL;
  }
  strcpy(ret->nicks[0], nick1);
  ret->nicks[1] = (char *)balloc(nicklen[1]);
  if(!ret->nicks[1])
  {
    bidentd_destroy(ret->identd);
    bfree(ret->info->server);
    bfree(ret->info);
    bfree(ret->server);
    bfree(ret->nicks[0]);
    bfree(ret);
    ret = NULL;
    return NULL;
  }
  strcpy(ret->nicks[1], nick2);
  ret->nicks[2] = (char *)balloc(nicklen[2]);
  if(!ret->nicks[2])
  {
    bidentd_destroy(ret->identd);
    bfree(ret->info->server);
    bfree(ret->info);
    bfree(ret->server);
    bfree(ret->nicks[0]);
    bfree(ret->nicks[1]);
    bfree(ret);
    ret = NULL;
    return NULL;
  }
  strcpy(ret->nicks[2], nick3);
  
  ret->username = (char *)balloc(strlen(username) + 1);
  if(!ret->username)
  {
    bidentd_destroy(ret->identd);
    bfree(ret->info->server);
    bfree(ret->info);
    bfree(ret->server);
    bfree(ret->nicks[0]);
    bfree(ret->nicks[1]);
    bfree(ret->nicks[2]);
    bfree(ret);
    ret = NULL;
    return NULL;
  }
  strcpy(ret->username, username);
  
  if(password)
  {
    ret->password = (char *)balloc(strlen(password) + 1);
    if(!ret->password)
    {
      bidentd_destroy(ret->identd);
      bfree(ret->info->server);
      bfree(ret->info);
      bfree(ret->server);
      bfree(ret->nicks[0]);
      bfree(ret->nicks[1]);
      bfree(ret->nicks[2]);
      bfree(ret->username);
      bfree(ret);
      ret = NULL;
      return NULL;
    }
    strcpy(ret->password, password);
  }
  else
    ret->password = NULL;
  ret->realname = (char *)balloc(strlen(realname) + 1);
  if(!ret->realname)
  {
    bidentd_destroy(ret->identd);
    bfree(ret->info->server);
    bfree(ret->info);
    bfree(ret->server);
    bfree(ret->nicks[0]);
    bfree(ret->nicks[1]);
    bfree(ret->nicks[2]);
    bfree(ret->username);
    bfree(ret->password);
    bfree(ret);
    ret = NULL;
    return NULL;
  }
  strcpy(ret->realname, realname);
  
  ret->callbacks = NULL;
  
  /* at this point all our data is ready and we are connected to the server, lets "register" on the server */
  LOG("Registering on server");
  if(ret->password)
    sprintf(connect_cmd, "\r\nPASS %s\r\n", ret->password);
  else
    sprintf(connect_cmd, "\r\n");
  sprintf(connect_cmd, "%sNICK %s\r\nNICK %s\r\nNICK %s\r\nUSER %s %s %s :%s\r\n", connect_cmd, ret->nicks[2], ret->nicks[1], ret->nicks[0], ret->username, our_host, ret->server, ret->realname);
  bsock_send(ret->socket, connect_cmd, strlen(connect_cmd));
  
  /* request the MOTD for easy access later, if needed */
  //bsock_send_fmt(ret->socket, "MOTD\r\n");
  
  /* start the irc thread */
  bthread_start(ret->thread);
  
  return ret;
}

void birc_disconnect(birc *i, char *reason)
{
  if(i->socket->connected)
  {
    bsock_send_fmt(i->socket, "QUIT :%s\r\n", (reason ? reason : "Leaving..."));
    bsock_disconnect(i->socket);
  }
}

void birc_destroy(birc *i)
{
  birc_disconnect(i, "birc_destroy auto-disconnect");
  bfree(i->realname);
  bfree(i->password);
  bfree(i->username);
  bfree(i->nicks[2]);
  bfree(i->nicks[1]);
  bfree(i->nicks[0]);
  bfree(i->nicks);
  bfree(i->server);
  bsock_destroy(i->socket);
  bidentd_destroy(i->identd);
  bthread_destroy(i->thread);
  bfree(i->info->server);
  bfree(i->info);
  bfree(i);
}


void *bafirc_irc_thread(void *data)
{
  char buf[1024], tmp[1024], tmp2[1024];
  char *cur;
  int index, index2, offset, ret, len;
  unsigned char nc = 0;
#ifndef ON_WINDOWS
    //struct timeval yield_timeval;
    //yield_timeval.sec = 0;
    //yield_timeval.usec = 10;
#endif
  bthread *self = (bthrea *)data;
  birc *irc = (birc *)self->data;
  birc_message *msg;
  offset = index = index2 = 0;
  while(!self->die)
  {
    ret = recv(irc->socket->socket, buf + index, 1024 - index, MSG_DONTWAIT);
    if(ret > 0)
      index += ret;
    //else if(ret == -1 && get_errno() == BAF_WOULD_BLOCK)
      //YIELD();
    /*else
    {
      // they disconnected us.
      //printf("kthxbye");
      //self->die = TRUE;
      //irc->socket->connected = FALSE;
      //return NULL;
    }*/
    
    /* look for a complete message, if there is one, process it */    
    if(strchr(buf, '\r') != NULL)
    {
      cur = (strchr(buf, '\r') > strchr(buf, '\n') ? strchr(buf, '\r') : strchr(buf, '\n')) + 1; /* +1 to get past the \r or \n */
      /* get our message in tmp */
      strncpy(tmp, buf, (cur - buf));
      tmp[cur - buf] = '\0';
      /* remove it from the buffer and shift the buffer back */
      
      index = strlen(buf);
      
      cur = buf;
      len = 0;
      while(cur && !strchr("\r\n", *cur))
      {
        ++cur;
      }
      if(cur && strchr("\r\n", *cur))
        ++cur;
      if(cur)
        cur += 1;
      strncpy(tmp, buf, (cur - buf));
      tmp[cur - buf] = '\0';
      memmove(buf, buf + (cur - buf), 1024 - (cur - buf));
      memset(buf + (cur - buf), '\0', 1024 - (cur - buf));
      index = strlen(buf);
      
      //LOG("Got a message: ");
      LOG(tmp);
      
      msg = birc_parse(tmp);
      
      /* some hard coded proc's */
      if(strncasecmp(msg->command, "PING", 4) == 0)
      {
        printf("PING!\n");
        bafirc_sock_send_fmt(irc->socket, "PONG :%s\r\n", msg->params[0]);
      }/*
      if(strncasecmp(msg->command, "ERROR", 5) == 0)
      {
        LOG("Recieved error from server, disconnected from irc. More info follows:");
        LOG(msg->params[0]);
        irc->socket->connected = FALSE;
        self->die = TRUE;
      }
      /* MOTD procs *//*
      if(strncasecmp(msg->command, "375", 3) == 0)
      {
        LOG("MOTD starting...");
        irc->info->motd_i = 0;
      }
      if(strncasecmp(msg->command, "372", 3) == 0)
      {
        int local_i;
        LOG(msg->params[0]);
        while(irc->info->motd_i < 10240 && local_i < strlen(msg->params[0]))
        {
          irc->info->motd[irc->info->motd_i] = msg->params[0][local_i];
          ++local_i; ++irc->info->motd_i;
        }
      }
      if(strncasecmp(msg->command, "376", 3) == 0)
      {
        LOG("MOTD done.");
      }*/
      
      bcallback_handle(msg, irc);
      birc_destroy_message(msg);
    }
    
    //YIELD();
  }
  return NULL;
}


birc_message *birc_parse(char *msg)
{
  birc_message *ret;
  unsigned int i;
  const char *t1 = msg;
  const char *t2 = msg;
  char t3[255];
  
  ret = (birc_message *)balloc(sizeof(birc_message));
  if(!ret)
    return NULL;
  ret->num_params = 0;
  ret->message = (char *)balloc(strlen(ret->message));
  if(!ret->message)
  {
    bfree(ret);
    return NULL;
  }
  strcpy(ret->message, msg);
  
  if(*t1 == ':') /* is there a message prefix? */
  {
    t2 = ++t1;
    while(*t2 && !strchr(" !", *t2))
      ++t2;
    ret->nickname = (char *)balloc((t2 - t1) + 1);
    if(!ret->nickname)
    {
      bfree(ret->message);
      bfree(ret);
      return NULL;
    }
    strncpy(ret->nickname, t1, t2 - t1);
    if(*t2 == '!') /* is there an id/username? */
    {
      t1 = ++t2;
      while(*t2 && !strchr(" @", *t2))
        ++t2;
      ret->username = (char *)balloc((t2 - t1) + 1);
      if(!ret->username)
      {
        bfree(ret->nickname);
        bfree(ret->message);
        bfree(ret);
        return NULL;
      }
      strncpy(ret->username, t1, t2 - t1);
      if(*t2 == '@') /* is there a hostname? */
      {
        t1 = ++t2;
        while(*t2 && !isspace(*t2))
          ++t2;
        ret->hostname = (char *)balloc((t2 - t1) + 1);
        if(!ret->hostname)
        {
          bfree(ret->username);
          bfree(ret->nickname);
          bfree(ret->message);
          bfree(ret);
          return NULL;
        }
        strncpy(ret->hostname, t1, t2 - t1);
      }
    }
  }
  
  while(*t2 && isspace(*t2))
    ++t2;
  t1 = t2;
  while(*t2 && !isspace(*t2))
    ++t2;
  
  ret->command = (char *)balloc((t2 - t1) + 1);
  if(!ret->command)
  {
    bfree(ret->hostname);
    bfree(ret->username);
    bfree(ret->nickname);
    bfree(ret->message);
    bfree(ret);
    return NULL;
  }
  strncpy(ret->command, t1, t2 - t1);
  for(i = 0; i < strlen(ret->command); ++i)
    ret->command[i] = toupper(ret->command[i]); /* make the command uppercase */
  
  ret->params = NULL;
  ret->num_params = 0;
  
  while(*t2 && isspace(*t2))
    ++t2;
  t1 = t2;
  while(*t1 && !strchr("\r\n", *t1))
  {
    if(*t1 == ':')
    {
      ++t1;
      while(*t2)
        ++t2;
      strncpy(t3, t1, t2 - t1);
      t3[t2 - t1] = '\0';
      if(bafirc_internal__add_param(ret, t3) != 0)
      {
        int j;
        for(j = 0; j < ret->num_params; ++j)
          bfree(ret->params[j])
        bfree(ret->params);
        bfree(ret->command);
        bfree(ret->hostname);
        bfree(ret->username);
        bfree(ret->nickname);
        bfree(ret->message);
        bfree(ret);
        return NULL;
      }
      break;
    }
    else
    {
      while(*t2 && !isspace(*t2))
        ++t2;
      strncpy(t3, t1, t2 - t1);
      t3[t2 - t1] = '\0';
      if(bafirc_internal__add_param(ret, t3) != 0)
      {
        int j;
        for(j = 0; j < ret->num_params; ++j)
          bfree(ret->params[j])
        bfree(ret->params);
        bfree(ret->command);
        bfree(ret->hostname);
        bfree(ret->username);
        bfree(ret->nickname);
        bfree(ret->message);
        bfree(ret);
        return NULL;
      }
      while(*t2 && isspace(*t2))
        ++t2;
      t1 = t2;
    }
  }
  return ret;
}

int birc__add_param(birc_message *m, const char *param)
{
  char **tmp = brealloc((void *)m->params, sizeof(char*) * m->num_params + 1);
  if(!tmp)
    return -1;
  m->num_params++;
  m->params = tmp;
  m->params[m->num_params - 1] = (char *)balloc(strlen(param) + 1);
  if(!m->params[m->num_params - 1])
  {
    m->num_params--;
    return -1;
  }
  strcpy(m->parmas[m->num_params -1], param);
  return 0;
}

void birc_destroy_message(birc_message *m)
{
  int j;
  for(j = 0; j < ret->num_params; ++j)
    bfree(ret->params[j])
  bfree(ret->params);
  bfree(ret->command);
  bfree(ret->hostname);
  bfree(ret->username);
  bfree(ret->nickname);
  bfree(ret->message);
  bfree(ret);
}
