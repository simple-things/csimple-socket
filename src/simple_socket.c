#include <ctype.h>

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
	int fd;
	GUID WSARecvMsg_GUID = {0xf689d7c8,0x6f1f,0x436b,{0x8a,0x53,0xe5,0x4f,0xe3,0x51,0xc3,0x22}};
    if(initialized){
        return;
	}

    initialized = 1;
    status.wVersion = 0;
    WSAStartup(MAKEWORD(2,2), &status);
    atexit(socket_cleanup);
	fd = socket_ipv4(SOCKET_TCP);
	WSAIoctl(fd, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &WSARecvMsg_GUID, sizeof(WSARecvMsg_GUID),
            &WSARecvMsg, sizeof(void*), &temp, NULL, NULL);

	socket_close(fd);
    started = 1;
}

#else

void socket_env_init(){
	signal(SIGPIPE, SIG_IGN);
}

#endif

int socket_origin(int family, int type, int protocol){
	return (int)socket(family, type, protocol);
}

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

    return socket_origin(family, sock_type, protocol);
}
int socket_ipv4(SOCKET_TYPE type){
	return socket_open(AF_INET, type);
}

int socket_ipv6(SOCKET_TYPE type){
	return socket_open(AF_INET6, type);
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

int socket_sendto(int fd, const char* data, int length, int flag, const struct sockaddr *dest_addr, socklen_t addrlen){
	return sendto(fd, data, length, flag, dest_addr, addrlen);
}

int socket_recvfrom(int fd, char *buffer, int length, int flag, struct sockaddr *src_addr, socklen_t* addrlen){
	return recvfrom(fd, buffer, length, flag, src_addr, addrlen);
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

int socket_writev(int fd, const socket_iovec* vectors, int count){
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

int socket_accept(int fd, struct sockaddr* client, socklen_t* addrlen){
	return accept(fd, client, addrlen);
}

int socket_bind(int fd, const struct sockaddr* address, socklen_t addrlen){
	return bind(fd, address, addrlen);
}

int socket_connect(int fd, const struct sockaddr* server, socklen_t addrlen){
	return connect(fd, server, addrlen);
}

int socket_getlocalip(int fd, struct sockaddr* local, socklen_t* addrlen){
	return getsockname(fd, local, addrlen);
};

int socket_getremoteip(int fd, struct sockaddr* remote, socklen_t* addrlen){
	return getpeername(fd, remote, addrlen);
}

int socket_select(int maxfd, fd_set* readset, fd_set* writeset, fd_set* exceptset, const struct timeval* timeout){
#if defined(_MSWINDOWS_)
	return select(maxfd, readset, writeset, exceptset, timeout);
#else
	return select(maxfd, readset, writeset, exceptset, (struct timeval*)timeout);
#endif
}

/*
WSAPoll(from Vista) is buggy, see http://daniel.haxx.se/blog/2012/10/10/wsapoll-is-broken/, and mingw/cygwin use select to implement poll. 
*/
int socket_poll(struct pollfd* fds, unsigned long nfds, int timeout){
#if defined(_MSWINDOWS_)
	return 0;
#else
	return poll(fds, nfds, timeout);
#endif
}
