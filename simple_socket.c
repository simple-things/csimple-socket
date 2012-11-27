#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "simple_socket.h"

#if defined(_MSWINDOWS_)

static int started = 0;

static void socket_cleanup(void){
    if(started){
        WSACleanup();
	}
}

void socket_env_init(){
	static int initialized = 0;
    WSADATA status;

    if(initialized){
        return;
	}

    initialized = 1;
    status.wVersion = 0;
    WSAStartup(MAKEWORD(2,2), &status);
    atexit(socket_cleanup);
    started = 1;
}

#else
int socket_env_init(){
	signal(SIGPIPE, SIG_IGN);
}
#endif
static int socket_open(int family, SOCKET_TYPE type){
    int sock_type, protocol;
    if(type == SOCKET_TCP) {
        sock_type = SOCK_STREAM;
        protocol = IPPROTO_TCP;
    } else if(type == SOCKET_UDP){
        sock_type = SOCK_DGRAM;
        protocol = IPPROTO_UDP;
    } else {
        sock_type = SOCK_RAW;
        protocol = 0;
    }

    return (int)socket(family, sock_type, protocol);
}
int socket_ipv4(SOCKET_TYPE type){
	return socket_open(AF_INET, type);
}

int socket_ipv6(SOCKET_TYPE type){
	return socket_open(AF_INET6, type);
}

static int socket_setblock(int fd, unsigned long block){
#if defined(_MSWINDOWS_)  
    return ioctlsocket(fd, FIONBIO, &block);
#else
    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    if (block == 1){
        flags |= ~O_NONBLOCK;
    } else{
        flags |= O_NONBLOCK;
    } 
    return fcntl(fd, F_SETFL, flags);
#endif
}

int socket_unblock(int fd){
	return socket_setblock(fd, 0);
}

int socket_block(int fd){
	return socket_setblock(fd, 1);
}

void socket_close(int fd){
#if defined(_MSWINDOWS_)
	closesocket(fd);
#else
	close(fd);
#endif
}

void socket_shutdown(int fd, int how){
	shutdown(fd, how);
}

int socket_send(int fd, const char* data, int length, int flag){
	return send(fd, data, length, flag);
}

int socket_recv(int fd, char* buffer, int length, int flag){
	return recv(fd, buffer, length, flag);
}

int socket_sendto(int fd, const char* data, int length, int flag, const struct sockaddr *dest_addr){
	return sendto(fd, data, length, flag, dest_addr, sizeof(*dest_addr));
}

int socket_recvfrom(int fd, char *buffer, int length, int flag, struct sockaddr *src_addr){
	int size = sizeof(*src_addr);
	return recvfrom(fd, buffer, length, flag, src_addr, &size);
}

int socket_sendmsg(int fd, socket_message* message, int flags){
#if defined(_MSWINDOWS_)
	return 0; //WSASendMsg(fd, (WSAMSG*)message, flags, 
#else
	return sendmsg(fd, (struct msghdr*)message, flags);
#endif	
}

int socket_recvmsg(int fd, socket_message* message, int flags){
#if defined(_MSWINDOWS_)
	return 0; //WSARecvMsg(fd, (WSAMSG*)message, flags, 
#else
	return recvmsg(fd, (struct msghdr*)message, flags);
#endif	
}

int socket_readv(int fd, socket_iovec* vectors, int count){
#if defined(_MSWINDOWS_)
    DWORD recv_bytes = 0;
    WSABUF* writes = (WSABUF*)vectors;
    DWORD flags = 0;
    return WSARecv(fd, writes, count, &recv_bytes, &flags, NULL, NULL);
#else
    struct iovec* reads = (struct iovec*)vectors;
    return readv(fd, reads, count);
#endif
}

int socket_writev(int fd, socket_iovec* vectors, int count){
#if defined(_MSWINDOWS_)
    DWORD sent_bytes = 0;
    WSABUF* writes = (WSABUF*)vectors;
    return WSASend(fd, writes, count, &sent_bytes, 0, NULL, NULL);
#else
    struct iovec* writes = (struct iovec*)vectors;
    return writev(fd, writes, count);
#endif
}
