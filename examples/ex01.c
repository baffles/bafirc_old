#include "bafirc.h"

int privmsg_proc(bcallback_info *inf)
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
      bsock_send_fmt(inf->irc->socket, "PRIVMSG #iia :Dying cause %s said !die.\r\n", inf->message->nickname);
      bsock_send_fmt(inf->irc->socket, "QUIT :I'll be back</accent>\r\n");
      //Sleep(1000);
      return -1;
    }
    if(strncasecmp(":!test", inf->message->params[i], 6) == 0 || strncasecmp("!test", inf->message->params[i], 5) == 0)
    {
      printf("!test !\n");
      bsock_send_fmt(inf->irc->socket, "PRIVMSG %s :Heh, %s said test! We are in the proc function. Your nick is %s, username is %s, hostname %s, and i can tell you lots more! Enjoy\r\n", inf->message->params[0], inf->message->nickname, inf->message->nickname, inf->message->username, inf->message->hostname);
    }
    if(strncasecmp(":!lag", inf->message->params[i], 5) == 0)
    {
      printf("!lag\n");
      bsock_send_fmt(inf->irc->socket, "PRIVMSG %s :My lag is %d\r\n", inf->message->nickname, inf->irc->lag);
    }
    if(strncasecmp("!bcom ", inf->message->params[i], 6) == 0)
    {
      printf("!bcom!\n");
      strncpy(bcomstuff, inf->message->params[i] + 6, strlen(inf->message->params[i]) - 6);
      bsock_send_fmt(inf->irc->socket, "%s\r\n", bcomstuff);
    }
    //void print_mem_report_irc(struct birc *irc, char *chan)
    if(strncasecmp("!mem", inf->message->params[i], 4) == 0)
    {
      printf("MEMORY REPORT!\n");
      print_mem_report_irc(inf->irc, inf->message->params[0]);
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
  //char buf[10];
  UNUSED(argc);
  UNUSED(argv);
  birc *irc;
  char *msg = "\r\nJOIN #iia\r\nPRIVMSG #iia :hey!\r\n";
  printf("Hi\n");
  if(bafirc_init())
  {
    printf("ERROR! %s\n", bafirc_error);
  }
  bcallback_global_install(privmsg_proc, "PRIVMSG");
  print_mem_report(stdout);
  irc = birc_connect("irc.us.freenode.net", 6667, "bafirc", "bafirc", "bafirc", "bafirc", "bafirc", NULL, TRUE);
  if(irc == NULL || !irc->socket->connected)
  {
    birc_destroy(irc);
    return -1;
  }
  bsock_send(irc->socket, msg, strlen(msg));
  print_mem_report(stdout);
  //sleep(1000);
  //sleep(30000);
  while(!irc->thread->die);
  //while(1);
  printf("bye");
  birc_disconnect(irc, "Ill be back!");
  birc_destroy(irc);
  print_mem_report(stdout);
  return 0;
}
	
