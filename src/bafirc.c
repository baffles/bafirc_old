/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** bafirc.c
 **/

#include "bafirc.h"
#define balloc malloc
#define bfree free
#define brealloc realloc
#define bcalloc calloc
#define print_mem_report(a)
#define print_mem_report_irc(a, b)

FILE *bafirc_log_file;
char bafirc_error[MAX_ERROR_LENGTH];

/* Function: bafirc_init
 * Arguments: none
 * Description: initializes the bafirc library
 * Return: true if there was an error, and puts the error in bafirc_error, false if not
 */
BOOL bafirc_init()
{
#ifdef FORTIFY
 Fortify_EnterScope();
#endif
  BAFIRC_LOAD_LOG_FILE();
  LOG("Setting up BAFIRC");
  if(bsock_init() != 0)
  {
    LOG("Couldn't initialize BAFIRC Sockets Subsystem");
    sprintf(bafirc_error, "Couldn't initialize bafirc socket subsystem");
    return TRUE;
  }
  if(bcallback_init() != 0)
  {
    LOG("Couln't initialize BAFIRC Callback Subsystem");
    sprintf(bafirc_error, "Couln't initialize bafirc callback subsystem");
    return TRUE;
  }
  atexit(bafirc_deinit);
  return FALSE;
}

/* Function: bafirc_deinit
 * Arguments: none
 * Description: deinitializes the bafirc library
 * Return: nothing
 */
void bafirc_deinit()
{
  // deinit stuff not needed yet, as the socket deinit takes care of itself
  BAFIRC_CLOSE_LOG_FILE();
#ifdef FORTIFY
 	Fortify_LeaveScope();
	Fortify_OutputStatistics(); 
#endif
}

int get_errno()
{
 #ifdef ON_WINDOWS
  return WSAGetLastError();
 #else
  return errno;
 #endif
}
