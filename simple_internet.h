#ifndef __CSIMPLE_INTERNET_H
#define __CSIMPLE_INTERNET_H

//todo add usage comment for every function.
#include <stdint.h>

#include "simple_platform.h"
#include "network_header.h"
#include "network_error.h"

#ifdef	__cplusplus
extern "C"
{
#endif

const char* ipv4_ntoa(struct in_addr addr);

int ipv4_aton(const char* addr, struct in_addr* result);

in_addr_t ipv4_addr(const char* addr);

char* ipv4_ntop(const void* addr, char* outbuffer, int bufferlen);

int ipv4_pton(const char* addr, void* result);

char* ipv6_ntop(const void* addr, char* outbuffer, int bufferlen);

int ipv6_pton(const char* addr, void* result);

const struct hostent* ipv4_gethostbyname(const char* hostname);

const struct hostent* ipv4_gethostbyaddr(const char* addr);

int ip_getaddrinfo(const char* hostname, const char* service, 
				   const struct addrinfo* hints, 
				   struct addrinfo** results);

void ip_freeaddrinfo(struct addrinfo* addrinfos);

int ip_getnameinfo(const struct sockaddr *sa, socklen_t salen,
                       char *host, size_t hostlen,
                       char *serv, size_t servlen, int flags);

const char* ip_gai_strerror(int error);

#ifdef	__cplusplus
}
#endif

#endif