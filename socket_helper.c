#include "socket_helper.h"
#include "simple_socket.h"
#include "simple_internet.h"

void simple_gettimeofday(struct timeval* time, void* local);

#define CHAR_IS_IPV6(addr) (strchr(addr, ':') != NULL)
#define SOCKSTORAGE_IS_IPV4(addr) (addr.ss_family == AF_INET)
#define SOCK_IS_IPV4(addr) (addr.sa_family == AF_INET)

int ipv4_addrinit(struct sockaddr* addr, const char* address, int port){
	struct sockaddr_in* addr4 = (struct sockaddr_in*)addr;
	simple_bzero(addr4, sizeof(*addr4));
	addr4->sin_family = AF_INET;
	addr4->sin_port = htons(port);
	if(ipv4_pton(address, &addr4->sin_addr) <= 0){
		return -1;
	}
	return 0;
}

int ipv6_addrinit(struct sockaddr* addr, const char* address, int port){
	struct sockaddr_in6* addr6 = (struct sockaddr_in6*)addr;
	simple_bzero(addr6, sizeof(*addr6));
	addr6->sin6_family = AF_INET6;
	addr6->sin6_port = htons(port);
	if(ipv6_pton(address, &addr6->sin6_addr) <= 0){
		return -1;
	}
	return 0;
}

int simple_addrinit(struct sockaddr* addr, const char* address, int port){
	if(CHAR_IS_IPV6(address)){
		return ipv6_addrinit(addr, address, port);
	}else{
		return ipv4_addrinit(addr, address, port);
	}
}

int simple_connect(int fd, const char* address, int port){
	struct sockaddr_storage addr;
	int len = 0;
	if(simple_addrinit((struct sockaddr*)&addr, address, port) != 0){
		return -1;
	}
	if(SOCKSTORAGE_IS_IPV4(addr)){
		len = sizeof(struct sockaddr_in);
	} else{
		len = sizeof(struct sockaddr_in6);
	}
	return socket_connect(fd, (struct sockaddr*)&addr, len);
}

int simple_bind(int fd, const char* address, int port){
	struct sockaddr_storage addr;
	int len = 0;
	if(simple_addrinit((struct sockaddr*)&addr, address, port) != 0){
		return -1;
	}
	if(SOCKSTORAGE_IS_IPV4(addr)){
		len = sizeof(struct sockaddr_in);
	} else{
		len = sizeof(struct sockaddr_in6);
	}
	return socket_bind(fd, (struct sockaddr*)&addr, len);
}

int simple_accept(int fd){
	int ret;
	for(;;){
		if( (ret = socket_accept(fd, NULL, NULL)) <= 0 && IS_INTERRUPTED){
			continue;
		} else{
			break;
		}
	}
	return ret;
}

int simple_pton(const char* addr, void* result){
	if(CHAR_IS_IPV6(addr)){
		return ipv6_pton(addr, result);
	}else{
		return ipv4_pton(addr, result);
	}
}

const char* simple_ntop(const void* addr, char* outbuffer, int bufferlen){
	struct sockaddr_storage* sockaddr = (struct sockaddr_storage*)addr;
	if(SOCKSTORAGE_IS_IPV4((*sockaddr))){
		return ipv4_ntop(addr, outbuffer, bufferlen);
	}else{
		return ipv6_ntop(addr, outbuffer, bufferlen);
	}
}

//-----------------
//copy from unp 
int socket_readn(int fd, char* buffer, int length){
	int	nleft;
	int	nread;
	char* ptr;

	ptr = buffer;
	nleft = length;
	while (nleft > 0) {
		if ( (nread = socket_recv(fd, ptr, nleft, 0)) < 0) {
			if (IS_INTERRUPTED)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(length - nleft);		/* return >= 0 */
}

#define MAXLINE 1024
static int my_read(int fd, char *ptr){
	int	read_cnt = 0;
	char	*read_ptr;
	char	read_buf[MAXLINE];
	if (read_cnt <= 0) {
again:
		if ((read_cnt = socket_recv(fd, read_buf, sizeof(read_buf), 0)) < 0) {
			if (IS_INTERRUPTED)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

int socket_readline(int fd, char* buffer, int length){
	int	n, rc;
	char	c, *ptr;

	ptr = buffer;
	for (n = 1; n < length; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

int socket_writen(int fd, const char* data, int length){
	int		nleft;
	int		nwritten;
	const char	*ptr;

	ptr = data;
	nleft = length;
	while (nleft > 0) {
		if ( (nwritten = socket_send(fd, ptr, nleft, 0)) <= 0) {
			if (nwritten < 0 && IS_INTERRUPTED)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(length);
}