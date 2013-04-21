#ifndef __CSOCKET_OPTIONS_H
#define __CSOCKET_OPTIONS_H

#include "simple_platform.h"
#include "network_header.h"
#include "network_error.h"

#ifdef	__cplusplus
extern "C"
{
#endif

int socket_unblock(int fd);

int socket_block(int fd);

/*
int socket_tcpdelay(int fd);

int socket_tcpnodelay(int fd);
*/
int socket_setoption(int fd, int level, int optname, const void* optval, socklen_t* optlen);

int socket_getoption(int fd, int level, int optname, void* optval, socklen_t* optlen);

#ifdef	__cplusplus
}
#endif

#endif