#include "socket_options.h"


int socket_setoption(int fd, int level, int optname, const void* optval, socklen_t* optlen){
	return setsockopt(fd, level, optname, (const char*)optval, *optlen);
}

int socket_getoption(int fd, int level, int optname, void* optval, socklen_t* optlen){
	return getsockopt(fd, level, optname, (char*)optval, optlen);
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
