#ifndef __CSOCKET_HELPER_H
#define __CSOCKET_HELPER_H

//most copy from unp

#include <stdint.h>
#include <string.h>

#include "simple_platform.h"
#include "network_header.h"
#include "network_error.h"

#ifdef	__cplusplus
extern "C"
{
#endif

#define STRING_IS_IPV6(addr) (strchr(addr, ':') != NULL)

#define SOCKSTORAGE_IS_IPV4(addr) (addr.ss_family == AF_INET)

#define SOCK_IS_IPV4(addr) (addr->sa_family == AF_INET)

void simple_gettimeofday(struct timeval* time, void* local);

int ipv4_addrinit(struct sockaddr* addr, const char* address, int port);

int ipv6_addrinit(struct sockaddr* addr, const char* address, int port);

int simple_socketwithaddrinfo(struct addrinfo* addr);

int simple_addrinit(struct sockaddr* addr, const char* address, int port);

int simple_connect(int fd, const char* address, int port);

int simple_connectwithaddrinfo(int fd, const struct addrinfo* addr);

int simple_sendtowithaddrinfo(int fd, const char* data, int length, int flag, const struct addrinfo* addr);

int simple_bind(int fd, const char* address, int port);

int simple_accept(int fd);

int simple_pton(const char* addr, void* result);

const char* simple_ntop(const void* addr, char* outbuffer, int bufferlen);

#define	simple_bzero(ptr,n)	memset(ptr, 0, n)

int socket_readn(int fd, char* buffer, int length);

int socket_readline(int fd, char* buffer, int length);

int socket_writen(int fd, const char* data, int length);

#ifdef	__cplusplus
}
#endif

#endif