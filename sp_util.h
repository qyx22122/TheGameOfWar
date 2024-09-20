#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Send functions
static int sendSize(int sock, size_t size) {
	int ret = send(sock, &size, sizeof(size), 0);

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

	int ret = send(sock, &x, size, 0);

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

	int ret = send(sock, &x, size, 0);

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

	int ret = send(sock, &x, size, 0);

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

	int ret = send(sock, x, size, 0);

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

	int ret = send(sock, x, size, 0);

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
	
	*value = malloc(size);

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
