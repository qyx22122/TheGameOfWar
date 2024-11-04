#pragma once

#if defined WIN32
// Fix for raylib
#define NOGDI // All GDI defines and routines
#define NOUSER // All USER defines and routines
#define NOSOUND // Sound driver routines
#define NOICONS // IDI_*

typedef struct point {
  long int x;
  long int y;
} point_w;

typedef struct tagMSG {
  long int hwnd;
  unsigned int message;
  unsigned int wParam;
  long int lParam;
  unsigned long int time;
  point_w pt;
  unsigned long int lPrivate;
} MSG, *PMSG, *NPMSG, *LPMSG;

#include <windows.h>
#include <mmsystem.h>

#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>

// Fix for raylib
#undef near
#undef far
#undef tagMSG

// Fix for send
#define MSG_NOSIGNAL 0

#else
#define closesocket close
#define SOCKET_ERROR -1
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static void clearWinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void closes(int sockfd) {
  closesocket(sockfd);
  clearWinsock();
}
void closew(int sockfd) {
  closesocket(sockfd);
}

// Initialize server socket & listen for connections
int initss(int* sockfd, int port) {

#if defined WIN32
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (res != 0) {
		perror("Error at WSASturtup.\n");
		return -1;
	}
#endif

  *sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int option = 1;
	
	if(*sockfd == -1) {
		perror("Couldn't create socket.\n");
    closes(*sockfd);
		return -1;
	}

#if !(defined WIN32)
	// Reuse address - stops adress already in use
	setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

#endif

	struct sockaddr_in server = {AF_INET, htons(port)};
	
	if(bind(*sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) == -1) {
		perror("Couldn't bind socket.\n");
		closes(*sockfd);
    return -1;
	}

	if(listen(*sockfd, 5) == -1) {
		perror("No ears. (Couldn't listen)\n");
		closes(*sockfd);
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
			closes(*newsockfd);
      return -1;
		}
		else if(*newsockfd == -1)
      return 1;

    return 0;
}
// Initialize client socket & connect to server
int initcs(int* sockfd, char* ip, int port) {

#if defined WIN32
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (res != 0) {
		perror("Error at WSASturtup.\n");
		return -1;
	}
#endif

	*sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(*sockfd == -1) {
		perror("Couldn't create socket.\n");
		closes(*sockfd);
    return -1;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip);

	printf("Connecting to %s:%d\n", ip, port);

	if(connect(*sockfd, (struct sockaddr*)&serverAddr, (socklen_t)sizeof(serverAddr)) == -1) {
		perror("Couldn't connect to server.\n");
		closes(*sockfd);
    return -1;
	}

  return 0;

}

// Send functions
static int sendSize(int sock, size_t size) {
	int ret = send(sock, (char*)&size, sizeof(size), MSG_NOSIGNAL);
  if(ret == SOCKET_ERROR) {
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

	int ret = send(sock, (char*)&x, size, MSG_NOSIGNAL);

	if(ret == SOCKET_ERROR) {
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

	int ret = send(sock, (char*)&x, size, MSG_NOSIGNAL);

	if(ret == SOCKET_ERROR) {
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

	int ret = send(sock, (char*)&x, size, MSG_NOSIGNAL);

	if(ret == SOCKET_ERROR) {
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

	if(ret == SOCKET_ERROR) {
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

	int ret = send(sock, (char*)x, size, MSG_NOSIGNAL);

	if(ret == SOCKET_ERROR) {
		perror("Sending failed.\n");
		return -1;
	}

	return ret;
}

// Recive functions

static size_t recvSize(int sock) {
	size_t size = 0;
	ssize_t ret = recv(sock, (char*)&size, sizeof(size), 0);

	if(ret == SOCKET_ERROR) {
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

	ssize_t ret = recv(sock, (char*)value, size, 0);

	if(ret == SOCKET_ERROR) {
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

	ssize_t ret = recv(sock, (char*)value, size, 0);

	if(ret == SOCKET_ERROR) {
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

	ssize_t ret = recv(sock, (char*)value, size, 0);

	if(ret == SOCKET_ERROR) {
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

	if(ret == SOCKET_ERROR) {
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

	ssize_t ret = recv(sock, (char*)value, size, 0);

	if(ret == SOCKET_ERROR) {
		perror("Reciving failed.\n");
		return -1;
	}
	else if(ret == 0) {
		printf("Reciving failed (Disconnected).\n");
		return 0;
	}

	return ret;
}
