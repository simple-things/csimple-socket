#ifndef __CSIMPLE_SOCKET_H
#define __CSIMPLE_SOCKET_H

//todo add usage comment for every function.

#include "simple_platform.h"
#include "network_header.h"
#include "network_error.h"

#ifdef	__cplusplus
extern "C"
{
#endif

void socket_env_init();

typedef enum{
	SOCKET_TCP,
	SOCKET_UDP,
	SOCKET_RAW
}SOCKET_TYPE;

int socket_origin(int family, int type, int protocol);

int socket_ipv4(SOCKET_TYPE type);

int socket_ipv6(SOCKET_TYPE type);

void socket_close(int fd);

void socket_shutdown(int fd, int how);

int socket_send(int fd, const char* data, int length, int flag);

int socket_recv(int fd, char* buffer, int length, int flag);

int socket_sendto(int fd, const char* data, int length, int flag, const struct sockaddr *dest_addr, socklen_t addrlen);

int socket_recvfrom(int fd, char *buffer, int length, int flag, struct sockaddr *src_addr, socklen_t* addrlen);

/* Data block for vector input, match struct iovec*/
typedef struct{
#if defined(_MSWINDOWS_)
	unsigned long length;
	char* data;
#else	
	char* data;
	size_t length;
#endif
}socket_iovec;

typedef struct {
#if defined(_MSWINDOWS_)
	struct sockaddr*	name;
	socklen_t			name_length;
	socket_iovec*		buffers;
	unsigned long		buffer_count;
	unsigned long		control_length;
	char*				control;
	unsigned long		flags;
#else
	struct sockaddr*	name;
    socklen_t			name_length;
    socket_iovec*		buffers;
    size_t				buffer_count;
	char*				control;			
	size_t				control_length;
    int					flags;
#endif
}socket_message;

int socket_sendmsg(int fd, socket_message* message, int flags);

int socket_recvmsg(int fd, socket_message* message, int flags);

int socket_writev(int fd, const socket_iovec* dataarray, int count);

int socket_readv(int fd, socket_iovec* dataarray, int count);

int socket_listen(int fd, int backlog);

int socket_accept(int fd, struct sockaddr* client, socklen_t* addrlen);

int socket_bind(int fd, const struct sockaddr* address, socklen_t addrlen);

int socket_connect(int fd, const struct sockaddr* server, socklen_t addrlen);

int socket_getlocalip(int fd, struct sockaddr* local, socklen_t* addrlen);

int socket_getremoteip(int fd, struct sockaddr* remote, socklen_t* addrlen);

int socket_select(int maxfd, fd_set* readset, fd_set* writeset, fd_set* exceptset, const struct timeval* timeout);

//int socket_poll(struct pollfd* fds, unsigned long nfds, int timeout);

#ifdef	__cplusplus
}
#endif

#endif

