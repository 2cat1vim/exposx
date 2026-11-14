#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SOCKET_FAIL -1
#define BIND_FAIL -1
#define LISTEN_FAIL -1
#define ACCEPT_FAIL -1
#define RECV_FAIL -1
#define FCNTL_GETFL_FAIL 0
#define FCNTL_SETFL_FAIL -1
#define MY_HOME_SERVER_DONT_BURN 1
#define BUFFER_SIZE 64

int main(int ac, char **av) {
	if (ac < 2) {
		fprintf(stderr, "exposx_server : args < 2 : missing port\n");
		exit(EXIT_FAILURE);
	}
	int port = atoi(av[1]);
	if (port <= 0) {
		fprintf(stderr, "exposx_server : atoi : port need to be a unsigned int\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "exposx_server : valid unsigned int `%d`\n", port);
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == SOCKET_FAIL) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "exposx_server : socket created `%d`\n", fd);
	struct sockaddr_in sa[1];
	memset(&sa, 0, sizeof(struct sockaddr*));
	sa->sin_family = AF_INET;
	sa->sin_port = htons(port);
	sa->sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(fd, (const struct sockaddr*)&sa, sizeof(sa)) == BIND_FAIL) {
		close(fd);
		perror("bind");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "exposx_server : bind success `%d`\n", fd);
	if (listen(fd, SOMAXCONN) == LISTEN_FAIL) {
		close(fd);
		perror("listen");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "exposx_server : listen success `%d`\n", fd);
	socklen_t lsa = sizeof(lsa);

	fd_set fds, readfds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	while (MY_HOME_SERVER_DONT_BURN) {
		readfds = fds;
		if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < -1) {
			close(fd);
			perror("select");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &readfds)) {
				if (i == fd) {	
					int client_fd = accept(fd, (struct sockaddr*)&sa, &lsa);
					if (client_fd == ACCEPT_FAIL) {
						close(fd);
						perror("accept");
						exit(EXIT_FAILURE);
					}
					FD_SET(client_fd, &fds);
				}
				else {
					char buffer[BUFFER_SIZE];
					size_t lbuffer = sizeof(buffer);
					if (recv(i, buffer, lbuffer, 0) == RECV_FAIL) {
						close(fd);
						perror("recv");
						exit(EXIT_FAILURE);
					}	
					close(i);
					fprintf(stdout, "client `%d`: %s\n", i, buffer);
					memset(buffer, 0, lbuffer);
				}
			}
		}
	}
	close(fd);
}
