#include "bafirc.h"

int privmsg_proc(bafirc_callback_info *inf)
{
  int i;
  char bcomstuff[512];/*
  printf("Parsed message %s\tResults:\n", inf->message->message);
  printf("Nickname:\t%s\n", inf->message->nickname);
  printf("Username:\t%s\n", inf->message->username);
  printf("Hostname:\t%s\n", inf->message->hostname);
  printf("Command:\t%s\n", inf->message->command);
  printf("Params (%d)\n", inf->message->num_params);
  for(i = 0; i < inf->message->num_params; ++i)
    printf("\t%d\t%s\n", i, inf->message->params[i]);*/
  printf("We got a privmsg!\n");
  for(i = 0; i < inf->message->num_params; ++i)
  {
    if(strncasecmp(":!die", inf->message->params[i], 5) == 0 || strncasecmp("!die", inf->message->params[i], 4) == 0)
    {
      printf("Dying\n");
      bafirc_sock_send_fmt(inf->irc->socket, "PRIVMSG #dsl :Dying cause %s said !die.\r\n", inf->message->nickname);
      bafirc_sock_send_fmt(inf->irc->socket, "QUIT :I'll be back</accent>\r\n");
      Sleep(1000);
      exit(0);
    }
    if(strncasecmp(":!test", inf->message->params[i], 6) == 0 || strncasecmp("!test", inf->message->params[i], 5) == 0)
    {
      printf("!test !\n");
      bafirc_sock_send_fmt(inf->irc->socket, "PRIVMSG %s :Heh, %s said test! We are in the proc function. Your nick is %s, username is %s, hostname %s, and i can tell you lots more! Enjoy\r\n", inf->message->params[0], inf->message->nickname, inf->message->nickname, inf->message->username, inf->message->hostname);
    }
    if(strncasecmp(":!lag", inf->message->params[i], 5) == 0)
    {
      printf("!lag\n");
      bafirc_sock_send_fmt(inf->irc->socket, "PRIVMSG %s :My lag is %d\r\n", inf->message->nickname, inf->irc->lag);
    }
    if(strncasecmp("!bcom ", inf->message->params[i], 6) == 0)
    {
      printf("!bcom!\n");
      strncpy(bcomstuff, inf->message->params[i] + 6, strlen(inf->message->params[i]) - 6);
      bafirc_sock_send_fmt(inf->irc->socket, "%s\r\n", bcomstuff);
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
  //char buf[10];
  UNUSED(argc);
  UNUSED(argv);
  bafirc_irc_connection *irc;
  char *msg = "\r\nJOIN #dsl\r\nPRIVMSG #dsl :hey!\r\n";
  bafirc_callback *cb = bafirc_callback_create(privmsg_proc, "privmsg");
  bafirc_callback_install(cb);
  if(bafirc_init())
  {
    printf("ERROR! %s\n", bafirc_error);
  }
  irc = bafirc_irc_connect("irc.choopa.net", 6667, "bafirc", "bafirc", "bafirc", "bafirc", "bafirc", NULL, TRUE);
  if(irc == NULL || !irc->socket->connected)
  {
    bafirc_destroy_irc(irc);
    return -1;
  }
  bafirc_sock_send(irc->socket, msg, strlen(msg));
  //sleep(1000);
  //sleep(30000);
  while(!irc->thread->die);
  printf("bye");
  bafirc_irc_disconnect(irc, "Ill be back!");
  bafirc_destroy_irc(irc);
  return 0;
}
	
