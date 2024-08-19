#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "sp_util.h"
#include "util.h"
#include "board.h"
#include "draw.h"

#define IP "127.0.0.1"
#define PORT 42042

#define VERSION "0.0.69"

void* drawGUI(void* n);
void* networking(void* n);

static Board board;
static bool turn;
static bool networkStatus;
static bool windowStatus;
static Move move;
static PlayerColor playerColor;
static pthread_t thread_NET;
static pthread_t thread_GUI;

static int sockfd;

int main() {
	
	move.type = NONE;
	turn = false;
	networkStatus = true;
	windowStatus = true;
	playerColor = NONE;


	pthread_create(&thread_GUI, NULL, drawGUI, NULL);


	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd == -1) {
		perror("Couldn't create socket.\n");
		networkStatus = false;
		return 1;
	}

	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(IP);

	printf("Connecting to %s:%d\n", IP, PORT);

	if(connect(sockfd, (struct sockaddr*)&serverAddr, (socklen_t)sizeof(serverAddr)) == -1) {
		perror("Couldn't connect to server.\n");
		networkStatus = false;
		return 1;
	}

	printf("Connected to server.\n");

	// Version Check
	char* version_buffer = VERSION;
	cspsend(sockfd, version_buffer);
	printf("Running version %s\n", VERSION);

	// Get player color from server
	char* color;
	csprecv(sockfd, &color);

	if(color[0] == 'E') {
		printf("Server Error -- %s\n", color);
		close(sockfd);
		networkStatus = false;
		return 1;
	}

	printf("Color: %s\n", color);

	if(color[0] == 'G') {
		playerColor = 0;
	}
	else if(color[0] == 'B') {
		playerColor = 1;
	}

	free(color);
	
	// Recive board update when game starts
	bzero(&board, sizeof(board));
	vsprecv(sockfd, (void*)&board);

	pthread_create(&thread_NET, NULL, networking, NULL);

	pthread_join(thread_GUI, NULL);

}
void* networking(void* n){
	while(true) {

		printf("Waiting for turn...\n");

		int ret = bsprecv(sockfd, &turn);
		
		printf("Turn: %d\n", turn);

		if(ret == 0) {
			printf("Server disconnected.\n");
			break;
		}

		if(turn) {
			while(move.type == NONE);

			printf("Sending move.\n");
			vspsend(sockfd, (void*)&move, sizeof(move));
			
			printf("Synchronizing with server.\n");
			char* errorMsg;
			csprecv(sockfd, &errorMsg);

			if(errorMsg[0] == 'E') {
				printf("Server Error -- %s\n", errorMsg);
				
				// Recive updated board
				vsprecv(sockfd, (void*)&board);
				continue;
			}
			
			if(errorMsg[0] != 'S') {
				printf("Error -- %s\n", errorMsg);
				break;
			}

			printf("Move sent successfully.\n");
		}

		printf("Reciving updated board.\n");

		vsprecv(sockfd, (void*)&board);
		
		char* gameState;
		ret = csprecv(sockfd, &gameState);

		if(ret == 0) {
			printf("Server disconnected.\n");
			break;
		}

		printf("Game state: %s\n", gameState);

		if(gameState[0] == 'C')
			continue;
		else if(gameState[0] == 'D') {
			printf("Game Ended in a DRAW.\n");
			break;
		}
		else if(gameState[0] == 'G') {
			printf("GREEN won.\n");
			break;
		}
		else if(gameState[0] == 'B') {
			printf("BLUE won.\n");
			break;
		}
		else {
			printf("Server Error: Server spat out hot garbage.\n");
			break;
		}
	}

	

	networkStatus = false;
	return 0;
}

void* drawGUI(void* n) {
	initWindow("The Game Of War", 800, 800);

	while(!windowShouldClose() && networkStatus) {
		move = drawBoard(&board, turn, (int)playerColor);
	}
	
	closeWindow();
	pthread_cancel(thread_NET);
	close(sockfd);
	return 0;
}
