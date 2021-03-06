#ifndef __CNETWORK_HEADER_H
#define __CNETWORK_HEADER_H

#include "simple_platform.h"

#if defined(_MSWINDOWS_)
    #include <WinSock2.h>
    #include <ws2tcpip.h>
	#include <Mswsock.h>
	#pragma comment(lib,"WS2_32.lib")
	#pragma comment(lib,"MsWSock.Lib")
    #define socket_errno ((int)WSAGetLastError())
    #define SOCKET_BAD INVALID_SOCKET
	#define SHUT_RD SD_RECEIVE
	#define SHUT_WR SD_SEND
	#define SHUT_RDWR SD_BOTH
#if !defined(_AFTER_VISTA)
struct pollfd {
    SOCKET  fd;
    SHORT   events;
    SHORT   revents;
};
#endif
typedef unsigned long in_addr_t ;
#else
    #include <sys/param.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <sys/uio.h>

    #include <netinet/in.h>
    #include <netinet/tcp.h>

	#include <arpa/inet.h>

    #include <fcntl.h>

    #include <ctype.h>
    #include <errno.h>
    #include <netdb.h>
    #include <pwd.h>
    #include <unistd.h>
	#include <signal.h>
	#include <poll.h>
    #define socket_errno      (errno)
    #define SOCKET_BAD -1
#endif

#define IS_INTERRUPTED (socket_errno == EINTR)

#endif

