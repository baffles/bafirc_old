/**
 ** BAFIRC : BAFSoft IRC Connection Library
 **
 ** 2005 BAF / BAFSoft
 **
 ** Please see readme.txt and license.txt for more info
 **
 ** socket.c
 **/

//#include "bafirc.h"
#include "bafirc/socket.h"

int bsock_send(bsock *s, char *buf, int len)
{
  int sent = 0, ls, data_len;
  data_len = (strlen(buf) > len ? len : strlen(buf));
  while(sent < data_len)
  {
    ls = send(s->socket, (const char *)buf + sent, data_len - sent, 0);
    if(ls <= 0)
      break;
    sent += ls;
  }
  return sent;
}

int bsock_send_fmt(bsock *s, char *fmt, ...)
{
  va_list marker;
  char buf[1024 * 4];
  
  va_start(marker, fmt);
  vsprintf(buf, fmt, marker);
  va_end(marker);
  
  return bsock_send(s, buf, strlen(buf));
}

int bsock_recv(bsock *s, char *buf, int len)
{
  int recvd, lr;
  for(recvd = 0; recvd < len; )
  {
    lr = recvd;
    if(s->non_blocking)
      recvd += recv(s->socket, buf + recvd, len - recvd, MSG_DONTWAIT);
    else
      recvd += recv(s->socket, buf + recvd, len - recvd, 0);
    if(recvd == lr)
      break;
  }
  return recvd;
}

bsock *bsock_connect(char *host, int port, unsigned char non_blocking)
{
  bsock *ret;
  struct hostent *server_addr;
  
  ret = (bsock *)balloc(sizeof(bsock));
  ret->hostname = (char *)balloc((strlen(host) + 1) * sizeof(char));
  strcpy(ret->hostname, host);
  ret->port = port;
  ret->socket = 0;
  ret->connected = 0;
  ret->non_blocking = non_blocking;
  
  LOG("Trying to resolve the server hostname");
  server_addr = gethostbyname(ret->hostname);
  if(!server_addr)
  {
    LOG("Couldn't find the hostname. Panicking.");
    return ret;
  }
  
  ret->destination.sin_family = AF_INET; // it is an internet socket
  ret->destination.sin_port = htons(ret->port); // put our port, into network byte order
  ret->destination.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)server_addr->h_addr))); // put in the ip we found...
  memset(&(ret->destination.sin_zero), '\0', 8); // clear the memory to NULL for the sin_zero crap... whatever
  
  ret->socket = socket(AF_INET, SOCK_STREAM, 0); // make the socket
  
  LOG("Trying to connect()...");
  if(connect(ret->socket, (struct sockaddr *)&ret->destination, sizeof(struct sockaddr)) == -1) // try and connect
  {
    LOG("Failed to connect(). More details follow:");
    switch(errno)
    {
      case EBADF:
        LOG("ret->socket is a bad file/socket descriptor.");
        break;
      case EFAULT:
        LOG("The socket is outside of our memory address space.");
        break;
      case ENOTSOCK:
        LOG("ret->socket is not associated with a socket.");
        break;
      case EISCONN:
        LOG("ret->socket is already connected.");
        break;
      case ECONNREFUSED:
        LOG("Connection refused to server.");
        break;
      case ETIMEDOUT:
        LOG("Connection timed out.");
        break;
      case ENETUNREACH:
        LOG("Network Unreachable.");
        break;
      case EADDRINUSE:
        LOG("Address already in use.");
        break;
      case EINPROGRESS:
        LOG("ret->socket is non blocking and something cannot be compeleted on it at the moment....");
        break;
      case EALREADY:
        LOG("ret->socket is non blocking and it is already trying to connect. ???");
        break;
      case EAFNOSUPPORT:
        LOG("sa_family is set wrong.");
        break;
      case EACCES:
        LOG("Trying to connect to a broadcast address...");
        break;
#ifdef ON_WINDOWS
      // check extra windows crap
      case WSANOTINITIALISED:
        LOG("A successful WSAStartup call must occur before using this function.");
        break;
      case WSAENETDOWN:
        LOG("The network subsystem has failed.");
        break;
      case WSAEINTR:
        LOG("The blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
        break;
      case WSAEADDRNOTAVAIL:
        LOG("The remote address is not a valid address (such as ADDR_ANY).");
        break;
      case WSAEINVAL:
        LOG("The parameter s is a listening socket.");
        break;
      case WSAENOBUFS:
        LOG("No buffer space is available. The socket cannot be connected.");
        break;
      case WSAEWOULDBLOCK:
        LOG("The socket is marked as nonblocking and the connection cannot be completed immediately.");
        break;
#endif
      default:
        LOG("Couldn't get any more information.");
        break;
    };
    return ret; // if we fail, return with everything, with connected set to false
  }
  ret->connected = 1; // yay we are connected!
  LOG("All connected up!");
  
  if(non_blocking)
  {
    LOG("The socket is non blocking");
#ifdef ON_WINDOWS
      LOG("We are on windows... we are setting the socket to non blocking.");
      u_long option = 1;
      if(ioctlsocket(ret->socket, FIONBIO, &option) == -1)
      {
        LOG("Failed with the ioctl().. that isn't too fatal though.");
      }
#endif
  }
  
  return ret; // return control
}

void bsock_disconnect(bsock *s)
{
#ifdef ON_WINDOWS
    closesocket(s->socket);
#else
    close(s->socket);
#endif /* ON_WINDOWS */
  s->connected = 0;
}

void bsock_destroy(bsock *s)
{
  if(s->connected)
    bsock_disconnect(s);
  if(s->hostname != NULL)
    bfree(s->hostname);
  s->hostname = NULL;
  s->port = 0;
  s->socket = 0;
  bfree(s);
}


int bsock_init()
{
#ifdef ON_WINDOWS
    WSADATA wsa_data;
    if(WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
      if(WSAStartup(MAKEWORD(1, 1), &wsa_data) != 0)
      {
        return -1;
      }
    }
    atexit(bsock_deinit);
#endif /* ON_WINDOWS */
  return 0;
}

void bsock_deinit()
{
#ifdef ON_WINDOWS
    WSACleanup();
#endif /* ON_WINDOWS */
}
