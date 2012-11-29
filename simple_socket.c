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

static LPFN_WSARECVMSG WSARecvMsg = NULL;
static void socket_cleanup(void){
    if(started){
        WSACleanup();
	}
}

void socket_env_init(){
	static int initialized = 0;
    WSADATA status;
	DWORD temp;
	int ret;
	int fd;
	GUID WSARecvMsg_GUID = WSAID_WSARECVMSG;
    if(initialized){
        return;
	}

    initialized = 1;
    status.wVersion = 0;
    WSAStartup(MAKEWORD(2,2), &status);
    atexit(socket_cleanup);
	fd = socket_ipv4(SOCKET_TCP);
	ret = WSAIoctl(fd, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &WSARecvMsg_GUID, sizeof(WSARecvMsg_GUID),
            &WSARecvMsg, sizeof(void*), &temp, NULL, NULL);

/*	if(ret != 0) {
		printf("load recvmsg fail %d ret error %d\n", ret, SOCKET_ERROR);
    }*/
	socket_close(fd);
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
        flags &= ~O_NONBLOCK;
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


int socket_readv(int fd, socket_iovec* vectors, int count){
#if defined(_MSWINDOWS_)
    DWORD recv_bytes = 0;
    DWORD flags = 0;
    int ret = WSARecv(fd, (WSABUF*)vectors, count, &recv_bytes, &flags, NULL, NULL);
	if(ret == 0){
		return recv_bytes;
	}
	return -1;
#else
    struct iovec* reads = (struct iovec*)vectors;
    return readv(fd, reads, count);
#endif
}

int socket_writev(int fd, socket_iovec* vectors, int count){
#if defined(_MSWINDOWS_)
    DWORD sent_bytes = 0;
    int ret =  WSASend(fd, (WSABUF*)vectors, count, &sent_bytes, 0, NULL, NULL);
	if(ret == 0){
		return sent_bytes;
	}
	return -1;
#else
    struct iovec* writes = (struct iovec*)vectors;
    return writev(fd, writes, count);
#endif
}

/**
 * Implementation of sendmsg() for WIN32
 *
 * We are using a contrived definition of msghdr which actually uses
 * an array of ::_WSABUF structures instead of ::iovec .  This allows
 * us to call WSASend directly using the given ::msghdr instead of
 * having to allocate another array of ::_WSABUF and copying data for
 * each call.
 *
 * Limitations:
 *
 * - msg->msg_name is ignored.  So is msg->control.
 * - WSASend() only supports ::MSG_DONTROUTE, ::MSG_OOB and
 *   ::MSG_PARTIAL.
 *
 * @param[in] s The socket to use.
 * @param[in] msg The message
 * @param[in] flags Flags.  
 *
 * @return The number of bytes sent, on success.  Or -1 on error.
 */

int socket_sendmsg(int fd, socket_message* message, int flags){
#if defined(_MSWINDOWS_)
	int ret;
    DWORD num_bytes_sent = 0;
#if defined(_AFTER_VISTA)
	ret = WSASendMsg(fd, (WSAMSG*)message, flags, &num_bytes_sent, NULL, NULL);
#else
/* Limitations:
 *
 * - msg->msg_name and msg->control are ignored.
 * - WSASend() only supports ::MSG_DONTROUTE, ::MSG_OOB and
 *   ::MSG_PARTIAL.
 */
 ret = WSASend(fd, (WSABUF*)message->buffers, message->buffer_count,
		  &num_bytes_sent, flags, NULL, NULL);
#endif
    if (ret == 0){
		return (int) num_bytes_sent;
	}

    /* srv == SOCKET_ERROR and WSAGetLastError() == WSA_IO_PENDING
       indicates that a non-blocking transfer has been scheduled.
       We'll have to check for that if we ever support non-blocking
       I/O. */
    return -1;
#else
	return sendmsg(fd, (struct msghdr*)message, flags);
#endif	
}

int socket_recvmsg(int fd, socket_message* message, int flags){
#if defined(_MSWINDOWS_)
	DWORD num_bytes_recv = 0;
	DWORD temp = flags;
	int ret;
	if(WSARecvMsg != NULL) {
		ret = WSARecvMsg(fd, (WSAMSG*)message, &num_bytes_recv, NULL, NULL);
	} else {
		ret = WSARecv(fd, (WSABUF*)message->buffers, message->buffer_count,
		  &num_bytes_recv, &temp, NULL, NULL);
	}
	if(ret != 0) {
		return -1;
	}
	return num_bytes_recv; 
#else
	return recvmsg(fd, (struct msghdr*)message, flags);
#endif	
}

int socket_listen(int fd, int backlog){
	return listen(fd, backlog);
}

int socket_accept(int fd, struct sockaddr* client){
	int len;
	return accept(fd, client, &len);
}

int socket_bind(int fd, const struct sockaddr* address){
	return bind(fd, address, sizeof(*address));
}

int socket_connect(int fd, const struct sockaddr* server){
	return connect(fd, server, sizeof(*server));
}

uint16_t simple_ntohs(uint16_t src){
	return ntohs(src);
}

uint16_t simple_htons(uint16_t src){
	return htons(src);
}

uint32_t simple_htonl(uint32_t src){
	return htonl(src);
}

uint32_t simple_ntohl(uint32_t src){
	return ntohl(src);
}

int socket_setoption(int fd, int level, int optname, void* optval, socklen_t* optlen){
#if defined(_MSWINDOWS_)
	return setsockopt(fd, level, optname, (const char*)optval, *optlen);
#else 
	return setsockopt(fd, level, optname, optval, optlen);
#endif	
}

int socket_getoption(int fd, int level, int optname, void* optval, socklen_t* optlen){
	return getsockopt(fd, level, optname, (char*)optval, optlen);
}

int socket_select(int maxfd, fd_set* readset, fd_set* writeset, fd_set* exceptset, const struct timeval* timeout){
	return select(maxfd, readset, writeset, exceptset, timeout);
}