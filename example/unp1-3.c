#include <stdio.h>
#include "../simple_socket.h"

#define MAX_LINE 128

int main(int argc, char const *argv[]) {
	int fd;
	char buffer[MAX_LINE+1];
	int n;
	int port = 13;
	if (argc < 2) {
		printf("expect argument ip address\n");
		return 0;
	}
	socket_env_init();
	fd = socket_ipv6(SOCKET_TCP);
	if(argc == 3) {
		port = atoi(argv2);
	}
	if (simple_connect(fd, argv[1], port) != 0) {
		printf("server connect failed\n");
		
	}

	while( (n=socket_recv(fd, buffer, MAX_LINE, 0)) > 0){
		buffer[n] = '\0';
		if (fputs(buffer, stdout) == EOF) {
			printf("output error\n");
		}
	}
	return 0;
}