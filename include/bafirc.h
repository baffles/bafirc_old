/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** bafirc.h
 **/

#ifndef BAFIRC_H
#define BAFIRC_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
  #define ON_WINDOWS
#endif /* WIN32 || WIN64 */

#if defined(ON_WINDOWS) && defined(BUILD_DLL)
  #define EXPORT __declspec(dllexport)
#else
  #if defined(ON_WINDOWS) && !defined(STATIC)
    #define EXPORT __declspec(dllimport)
  #else
    #define EXPORT extern
  #endif /* ON_WINDOWS ** !STATIC */
#endif /* ON_WINDOWS && BUILD_DLL */

#ifdef BAFIRC_DEBUG
  #define ASSERT(v) if(!(v)) { printf("Error caught!\n%s in %s on line %d\n", #v, __FILE__, __LINE__); exit(EXIT_FAILURE); }
#else
  #define ASSERT(v)
  #define LOG(v)
#endif

#define MAX_HOSTNAME 1024
#define MAXHOSTNAME 1024
#define MAX_ERROR_LENGTH 255
EXPORT char bafirc_error[MAX_ERROR_LENGTH];

#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

#ifdef ON_WINDOWS
  #undef BOOL
  // be safe with allegro
  #ifdef ALLEGRO_H
  #include <winalleg.h>
  #endif
  #include <windows.h>
  #include <winsock.h>
  #define BAF_WOULD_BLOCK WSAEWOULDBLOCK
  // connect errno shaz
  #define EBADF WSAEBADF
  #define EFAULT WSAEFAULT
  #define ENOTSOCK WSAENOTSOCK
  #define EISCONN WSAEISCONN
  #define ECONNREFUSED WSAECONNREFUSED
  #define ETIMEDOUT WSAETIMEDOUT
  #define ENETUNREACH WSAENETUNREACH
  #define EADDRINUSE WSAEADDRINUSE
  #define EINPROGRESS WSAEINPROGRESS
  #define EALREADY WSAEALREADY
  #define EAFNOSUPPORT WSAEAFNOSUPPORT
  #define EACCES WSAEACCES
  // done
  #define MSG_DONTWAIT 0
  //#include "bafirc/compat.h" /* Defines to make the Berkely Sockets code compatible with windows */
  //#define _errno WSAGetLastError()
  #define YIELD() Sleep(10)
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <errno.h>
  #include <sys/ioctl.h>
  #include <sys/select.h> 
  #define _errno errno
  //#define EWOULDBLOCK EAGAIN
  #define BAF_WOULD_BLOCK EAGAIN
  #define YIELD() select(0, NULL, NULL, NULL, yield_timeval)
#endif /* ON_WINDOWS */
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#define DEFAULT_NAMES "BAFIRC"
#define UNUSED(x) (void) x
#ifdef ON_WINDOWS
  #define usleep(x) Sleep(x)
#endif

EXPORT FILE *bafirc_log_file;

#if defined(BAFIRC_DEBUG) || defined(BAFIRC_USELOG)
  #define LOG(v)    fprintf(bafirc_log_file, "%s: %s\n", __FUNCTION__, v)
#endif

#if defined(BAFIRC_DEBUG) || defined(BAFIRC_USELOG)
  #define BAFIRC_LOAD_LOG_FILE() bafirc_log_file = fopen("./bafirc.log", "w+");
#else
  #define BAFIRC_LOAD_LOG_FILE()
#endif

#if defined(BAFIRC_DEBUG) || defined(BAFIRC_USELOG)
  #define BAFIRC_CLOSE_LOG_FILE() fclose(bafirc_log_file);
#else
  #define BAFIRC_CLOSE_LOG_FILE()
#endif

#include "bafirc/typedefs.h"

#include "bafirc/mem.h"
#include "bafirc/threads.h"

#include "bafirc/socket.h"
#include "bafirc/identd.h"
#include "bafirc/callback.h"
#include "bafirc/irc.h"
//#include "bafirc/rfc.h"

EXPORT BOOL bafirc_init();
EXPORT void bafirc_deinit();

EXPORT int get_errno();

#ifndef BAFIRC_USE_LOWLEVEL
  //#include "bafirc/api.h"
#endif /* BAFIRC_USE_LOWLEVEL */

#ifdef __cplusplus
};
#endif

#endif /* BAFIRC_H */
