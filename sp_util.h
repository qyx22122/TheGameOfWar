#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

// Initialize server socket & listen for connections
int initss(int* sockfd, int port) {
  *sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int option = 1;
	
	if(*sockfd == -1) {
		perror("Couldn't create socket.\n");
		return -1;
	}
	// Reuse address - stops adress already in use
	setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	struct sockaddr_in server = {AF_INET, htons(port)};
	
	if(bind(*sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) == -1) {
		perror("Couldn't bind socket.\n");
		return -1;
	}

	if(listen(*sockfd, 5) == -1) {
		perror("No ears. (Couldn't listen)\n");
		return -1;
	}

  return 0;

}
int acceptss(int sockfd, int* newsockfd) {
    struct sockaddr_in clientAddr;
		socklen_t clientAddrLenght = sizeof(clientAddr);

		*newsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLenght);
		
		if(*newsockfd == -1 && errno != 11) {
			perror("Couldn't accept the truth. (Accept failed)\n");
			return -1;
		}
		else if(*newsockfd == -1)
      return 1;

    return 0;
}
// Initialize client socket & connect to server
int initcs(int* sockfd, char* ip, int port) {
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(*sockfd == -1) {
		perror("Couldn't create socket.\n");
		return -1;
	}

	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip);

	printf("Connecting to %s:%d\n", ip, port);

	if(connect(*sockfd, (struct sockaddr*)&serverAddr, (socklen_t)sizeof(serverAddr)) == -1) {
		perror("Couldn't connect to server.\n");
		return -1;
	}

  return 0;

}

void closes(int sockfd) {
  close(sockfd);
}

// Send functions
static int sendSize(int sock, size_t size) {
	int ret = send(sock, &size, sizeof(size), MSG_NOSIGNAL);

	if(ret == -1) {
		perror("Sending size failed.\n");
		return -1;
	}

	return ret;
}

int ispsend(int sock, int x) {
	
	size_t size = sizeof(x);

	if(!sendSize(sock, size)) {
		
		printf("Sending failed.\n");
		return -1;
	}

	int ret = send(sock, &x, size, MSG_NOSIGNAL);

	if(ret == -1) {
		perror("Sending failed.\n");
		return -1;
	}

	return ret;
}
int fspsend(int sock, float x) {
	
	size_t size = sizeof(x);

	if(!sendSize(sock, size)) {
		
		printf("Sending failed.\n");
		return -1;
	}

	int ret = send(sock, &x, size, MSG_NOSIGNAL);

	if(ret == -1) {
		perror("Sending failed.\n");
		return -1;
	}

	return ret;

}
int bspsend(int sock, bool x) {
	size_t size = sizeof(x);

	if(!sendSize(sock, size)) {
		
		printf("Sending failed.\n");
		return -1;
	}

	int ret = send(sock, &x, size, MSG_NOSIGNAL);

	if(ret == -1) {
		perror("Sending failed.\n");
		return -1;
	}

	return ret;
}

int cspsend(int sock, char* x) {

	// null-terminator at the end of the string
	size_t size = (strlen(x) + 1) * sizeof(char);

	if(!sendSize(sock, size)) {
		
		printf("Sending failed.\n");
		return -1;
	}

	int ret = send(sock, x, size, MSG_NOSIGNAL);

	if(ret == -1) {
		perror("Sending failed.\n");
		return -1;
	}

	return ret;
}

int vspsend(int sock, void* x, size_t size) {
	if(!sendSize(sock, size)) {
		
		printf("Sending failed.\n");
		return -1;
	}

	int ret = send(sock, x, size, MSG_NOSIGNAL);

	if(ret == -1) {
		perror("Sending failed.\n");
		return -1;
	}

	return ret;
}

// Recive functions

static size_t recvSize(int sock) {
	size_t size = 0;
	ssize_t ret = recv(sock, &size, sizeof(size), 0);


	if(ret == -1) {
		perror("Reciving size failed.\n");
		return -1;
	}

	return ret != 0 ? size : -1;
}

int isprecv(int sock, int* value) {
	size_t size = recvSize(sock);
	if(size == -1) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	ssize_t ret = recv(sock, value, size, 0);

	if(ret == -1) {
		perror("Reciving failed.\n");
		return -1;
	}
	else if(ret == 0) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	return ret;
}
int fsprecv(int sock, float* value) {
	size_t size = recvSize(sock);
	if(size == -1) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	ssize_t ret = recv(sock, value, size, 0);

	if(ret == -1) {
		perror("Reciving failed.\n");
		return -1;
	}
	else if(ret == 0) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	return ret;
}
int bsprecv(int sock, bool* value) {
	size_t size = recvSize(sock);
	if(size == -1) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	ssize_t ret = recv(sock, value, size, 0);

	if(ret == -1) {
		perror("Reciving failed.\n");
		return -1;
	}
	else if(ret == 0) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	return ret;
}
int csprecv(int sock, char** value) {
	size_t size = recvSize(sock);
	if(size == -1) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}
	
	*value = (char*)malloc(size);

	if(*value == NULL) {
		printf("Reciving failed (Overflow).\n");
		printf("Cannot allocate %zu bytes for string.\n", size);
		return -1;
	}

	ssize_t ret = recv(sock, *value, size, 0);

	if(ret == -1) {
		perror("Reciving failed.\n");
		return -1;
	}
	else if(ret == 0) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	return ret;
}

int vsprecv(int sock, void* value) {
	size_t size = recvSize(sock);
	if(size == -1) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	ssize_t ret = recv(sock, value, size, 0);

	if(ret == -1) {
		perror("Reciving failed.\n");
		return -1;
	}
	else if(ret == 0) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	return ret;
}
