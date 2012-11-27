#ifndef __CSIMPLE_ERROR_H
#define __CSIMPLE_ERROR_H

#include "simple_platform.h"

#if defined(_MSWINDOWS_)
    #define EWOULDBLOCK             WSAEWOULDBLOCK
    #define EINPROGRESS             WSAEINPROGRESS
    #define EALREADY                WSAEALREADY
    #define ENOTSOCK                WSAENOTSOCK
    #define EDESTADDRREQ            WSAEDESTADDRREQ
    #define EMSGSIZE                WSAEMSGSIZE
    #define EPROTOTYPE              WSAEPROTOTYPE
    #define ENOPROTOOPT             WSAENOPROTOOPT
    #define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
    #define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
    #define EOPNOTSUPP              WSAEOPNOTSUPP
    #define EPFNOSUPPORT            WSAEPFNOSUPPORT
    #define EAFNOSUPPORT            WSAEAFNOSUPPORT
    #define EADDRINUSE              WSAEADDRINUSE
    #define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
    #define ENETDOWN                WSAENETDOWN
    #define ENETUNREACH             WSAENETUNREACH
    #define ENETRESET               WSAENETRESET
    #define ECONNABORTED            WSAECONNABORTED
    #define ECONNRESET              WSAECONNRESET
    #define ENOBUFS                 WSAENOBUFS
    #define EISCONN                 WSAEISCONN
    #define ENOTCONN                WSAENOTCONN
    #define ESHUTDOWN               WSAESHUTDOWN
    #define ETOOMANYREFS            WSAETOOMANYREFS
    #define ETIMEDOUT               WSAETIMEDOUT
    #define ECONNREFUSED            WSAECONNREFUSED
    #define ELOOP                   WSAELOOP
    #define ENAMETOOLONG            WSAENAMETOOLONG
    #define EHOSTDOWN               WSAEHOSTDOWN
    #define EHOSTUNREACH            WSAEHOSTUNREACH
    #define ENOTEMPTY               WSAENOTEMPTY
    #define EPROCLIM                WSAEPROCLIM
    #define EUSERS                  WSAEUSERS
    #define EDQUOT                  WSAEDQUOT
    #define ESTALE                  WSAESTALE
    #define EREMOTE                 WSAEREMOTE
    #define EINTR                   WSAEINTR
#endif

#endif

