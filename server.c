#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "util.h"

#define SIZE sizeof(struct sockaddr_in)

typedef struct Player {
	int connectionfd;
	struct sockaddr address;
} Player;

typedef struct Game {
	Board board;
	Player green;
	Player blue;
	int moveCount;
} Game;

void handle_player(int connectionfd, struct sockaddr addr);

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd == -1) {
		perror("Couldn't create socket.\n");
		return 1;
	}

	struct sockaddr_in server = {AF_INET, htons(atoi("42042"))};
	
	if(bind(sockfd, (struct sockaddr*)&server, SIZE) == -1) {
		perror("Couldn't bind socket.\n");
		return 1;
	}

	if(listen(sockfd, 5) == -1) {
		perror("No ears. (Couldn't listen)\n");
		return 1;
	}

	printf("Server has started.\n");

	while(true) {

		struct sockaddr newAddr;
		int newAddrLenght = -1;
		int newsockfd = accept(sockfd, NULL, NULL);
		
		if(newsockfd == -1) {
			perror("Couldn't accept the truth. (Accept failed)\n");
			return 1;
		}
		int pid = fork();
		if(pid == -1) {
			perror("Forked it. (Couldn't fork process)\n");
			return 1;
		}

		if(pid == 0) {
			handle_player(newsockfd, newAddr);
		}

		close(newsockfd);

	}

	return 0;
}

void handle_player(int connectionfd, struct sockaddr addr) {
	printf("Client connected (%s)\n", addr.sa_data);
	char* buffer = "Hello World";
	write(connectionfd, buffer, strlen(buffer)*sizeof(char));
}
