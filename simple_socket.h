#ifndef __CSIMPLE_SOCKET_H
#define __CSIMPLE_SOCKET_H

//todo add usage comment for every function.
#include <stdint.h>

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

int socket_ipv4(SOCKET_TYPE type);

int socket_ipv6(SOCKET_TYPE type);

void socket_close(int fd);

void socket_shutdown(int fd, int how);

int socket_send(int fd, const char* data, int length, int flag);

int socket_recv(int fd, char* buffer, int length, int flag);

int socket_sendto(int fd, const char* data, int length, int flag, const struct sockaddr *dest_addr);

int socket_recvfrom(int fd, char *buffer, int length, int flag, struct sockaddr *src_addr);

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
	int					name_length;
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

int socket_writev(int fd, socket_iovec* data_array, int count);

int socket_readv(int fd, socket_iovec* data_array, int count);

void socket_select();

void socket_poll();

int socket_listen();

int socket_accept();

int socket_bind();

int socket_connect();

int socket_unblock(int fd);

int socket_block(int fd);

/*
int socket_delay();

int socket_no_delay();
*/
int socket_set_option();

int socket_get_option();

uint16_t simple_ntohs();

uint16_t simple_htons();

uint32_t simple_htonl();

uint32_t simple_ntohl();

#ifdef	__cplusplus
}
#endif

#endif

