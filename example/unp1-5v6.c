#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "../simple_socket.h"
#include "../socket_helper.h"

#define MAX_LINE 128

int main(int argc, char const *argv[]) {
	int fd;
	char buffer[MAX_LINE+1];
	int n;
	int port = 13;
	int client;
	if (argc < 2) {
		printf("expect argument ip address\n");
		return 0;
	}
	socket_env_init();
	fd = socket_ipv6(SOCKET_TCP);
	if(argc == 3) {
		port = atoi(argv[2]);
	}
	if (simple_bind(fd, argv[1], port) != 0) {
		printf("server bind failed\n");
		return 0;
	}
	if (socket_listen(fd, 64) != 0) {
		printf("server listen failed\n");
		return 0;
	}
	while( (client=simple_accept(fd)) != -1){
		time_t ticks = time(NULL);
		simple_bzero(buffer, sizeof(buffer));
		sprintf(buffer, "%.24s\r\n", ctime(&ticks));
		socket_send(client, buffer, strlen(buffer), 0);
		socket_close(client);
	}
	return 0;
}