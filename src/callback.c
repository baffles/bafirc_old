/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2004 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** callback.c
 **/

#include "bafirc.h"

bcallback_list *bcallback_global_list = NULL;

int bcallback_global_install(bcallback_func func, char *trigger)
{
  bcallback_list *working, *node;
  if(!bcallback_global_list)
  {
    bcallback_global_list = (bcallback_list *)balloc(sizeof(bcallback_list));
    if(!bcallback_global_list)
      return -1;
    working = node = bcallback_global_list;
  }
  else
  {
    node = bcallback_global_list;
    while(1)
    {
      if(!node->next)
        break;
      node = node->next;
    }
    
    working = (bcallback_list *)balloc(sizeof(bcallback_list));
    if(!working)
      return -1;
    node->next = working;
  }
  working->function = func;
  working->trigger = (char *)balloc(strlen(trigger) + 1);
  if(!working->trigger)
    return -1;
  strcpy(working->trigger, trigger);
  working->next = NULL;
  return 0;
}

int bcallback_global_uninstall(bcallback_func func)
{
  bcallback_list *node = bcallback_global_list, *working = NULL, *prev = bcallback_global_list, *tmp;
  
  while(1)
  {
    if(node->function == func)
    {
      working = node;
      break;
    }
    else if(node->next == NULL)
      break;
    prev = node;
    node = node->next;
  }
  
  if(working == NULL) /* this wasn't found in the list, so just give up :P */
    return -1;
  node = working;
  
  /* otherwise delete and free it */
  bfree(node->trigger);
  tmp = node->next;
  if(node->next == NULL)
  {
    bfree(node);
    bcallback_global_list = NULL;
  }
  else if(node == prev)
  {
    bfree(node);
    bcallback_global_list = tmp;
  }
  else
  {
    node->next = NULL;
    bfree(prev->next);
    prev->next = tmp;
  }
  return 0;
}


int bcallback_install(birc *irc, bcallback_func func, char *trigger)
{
  bcallback_list *working, *node;
  if(!irc->callbacks)
  {
    irc->callbacks = (bcallback_list *)balloc(sizeof(bcallback_list));
    if(!irc->callbacks)
      return -1;
    working = node = irc->callbacks;
  }
  else
  {
    node = irc->callbacks;
    while(1)
    {
      if(!node->next)
        break;
      node = node->next;
    }
    
    working = (bcallback_list *)balloc(sizeof(bcallback_list));
    if(!working)
      return -1;
    node->next = working;
  }
  working->function = func;
  working->trigger = (char *)balloc(strlen(trigger) + 1);
  if(!working->trigger)
    return -1;
  strcpy(working->trigger, trigger);
  working->next = NULL;
  return 0;
}

int bcallback_uninstall(birc *irc, bcallback_func func)
{
  bcallback_list *node = irc->callbacks, *working = NULL, *prev = irc->callbacks, *tmp;
  
  while(1)
  {
    if(node->function == func)
    {
      working = node;
      break;
    }
    else if(node->next == NULL)
      break;
    prev = node;
    node = node->next;
  }
  
  if(working == NULL) /* this wasn't found in the list, so just give up :P */
    return -1;
  node = working;
  
  /* otherwise delete and free it */
  bfree(node->trigger);
  tmp = node->next;
  if(node->next == NULL)
  {
    bfree(node);
    bcallback_global_list = NULL;
  }
  else if(node == prev)
  {
    bfree(node);
    bcallback_global_list = tmp;
  }
  else
  {
    node->next = NULL;
    bfree(prev->next);
    prev->next = tmp;
  }
  return 0;
}

int bcallback_handle(birc_message *m, birc *irc)
{
  /* call all global + local callbacks for the message's command (ie, trigger) */
  bcallback_list *node = bcallback_global_list;
  bcallback_info *info = (bcallback_info *)balloc(sizeof(bcallback_info));
  info->message = m; info->irc = irc;
  if(node)
  {
    while(1)
    {
      if(strcasecmp(m->command, node->trigger) == 0)
        (*node->function)(info);
      if(!node->next)
        break;
      node = node->next;
    }
  }
  
  node = irc->callbacks;
  if(node)
  {
    while(1)
    {
      if(strcasecmp(m->command, node->trigger) == 0)
        (*node->function)(info);
      if(!node->next)
        break;
      node = node->next;
    }
  }
}


int bcallback_init()
{
  atexit(bcallback_deinit);
}

void bcallback_deinit()
{
  /* just clean up the global list */
  bcallback_list *node = bcallback_global_list, *tmp;
  if(node)
  {
    while(1)
    {
      if(node->next)
        tmp = node->next;
      bfree(node->trigger);
      bfree(node);
      if(!node->next)
        break;
      node = tmp;
    }
  }
}
