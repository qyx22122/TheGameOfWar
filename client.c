#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int main() {
	char* ip = "127.0.0.1";
	int port = 42042;
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd == -1) {
		perror("Couldn't create socket.\n");
		return 1;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip);

	if(connect(sockfd, (struct sockaddr*)&serverAddr, (socklen_t)sizeof(serverAddr)) == -1) {
		perror("Couldn't connect to server.\n");
		return 1;
	}

	printf("Connected to server.\n");

	char buffer[1024];
	bzero(buffer, 1024);
	
	recv(sockfd, buffer, sizeof(buffer), 0);

	printf("%s\n", buffer);

	close(sockfd);

	return 0;
}
