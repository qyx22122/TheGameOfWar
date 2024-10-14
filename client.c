#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "sp_util.h"
#include "util.h"
#include "board.h"
#include "draw.h"

#define VERSION "0.0.69"

void drawGUI();
void* connectToServer(void* n);

static Board board;
static bool turn;
static bool networkStatus = true;
static bool gameStarted;
static Move move;
static PlayerColor playerColor;
static pthread_t thread_NET;
static int playerWon;

static char* ip;
static uint16_t port;

static int sockfd;

int main() {
  drawGUI();
	return 0;
}
void* connectToServer(void* n){

	networkStatus = true;
	playerColor = NONE;
	gameStarted = false;
	playerWon = -1;
	turn = false;
	move.type = NONE;

  close(sockfd);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd == -1) {
		perror("Couldn't create socket.\n");
		networkStatus = false;
		return 0;
	}

	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip);

	printf("Connecting to %s:%d\n", ip, port);

	if(connect(sockfd, (struct sockaddr*)&serverAddr, (socklen_t)sizeof(serverAddr)) == -1) {
		perror("Couldn't connect to server.\n");
		networkStatus = false;
    close(sockfd);
		return 0;
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
		return 0;
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

	gameStarted = true;

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
			playerWon = 2;
			break;
		}
		else if(gameState[0] == 'G') {
			printf("GREEN won.\n");
			playerWon = 0;
			break;
		}
		else if(gameState[0] == 'B') {
			printf("BLUE won.\n");
			playerWon = 1;
			break;
		}
		else {
			printf("Server Error: Server spat out hot garbage.\n");
			break;
		}
	}

	networkStatus = false;
  gameStarted = false;
	close(sockfd);
	return 0;
}

void drawGUI() {
	initWindow("The Game Of War", 800, 800);

	ip = malloc(17*sizeof(char));

	if(ip == NULL) {
		perror("Couldn't allocate ip string.\n");
		return;
	}

	char* portStr = malloc(6*sizeof(char));
	if(portStr == NULL) {
		perror("Couldn't allocate port string.\n");
		return;
	}

	strcpy(ip, "127.0.0.1");
	strcpy(portStr, "42042");

	int ipLenght = strlen(ip);
	int portLenght = strlen(portStr);
	
  bool failedConnection = false;
  bool invalidAddress = false;
	
serverSelection:
  
	while(!windowShouldClose()) {
		bool ret = drawServerSelection(ip, &ipLenght, portStr, &portLenght, &invalidAddress, failedConnection);

		if(ret) {
			if(!isValidIp4(ip)) {
				printf("Invalid address.\n");
				invalidAddress = true;
				continue;
			}

			port = atoi(portStr);

			break;
		}
	}

	pthread_create(&thread_NET, NULL, connectToServer, NULL);
  networkStatus = true;

	while(!windowShouldClose() && !gameStarted && networkStatus) {
		drawLoading(ip, port);
	}

	if(!networkStatus && !windowShouldClose()) {
    printf("Failed to connect!\n");
		failedConnection = true;
    gameStarted = false;
		goto serverSelection;
	}
  
  printf("Game has started.\n");

	while(!windowShouldClose() && networkStatus) {
		move = drawBoard(&board, turn, (int)playerColor);
	}
	
  if(playerWon == -1 && !windowShouldClose()) {
    pthread_cancel(thread_NET);
    failedConnection = true;
    networkStatus = false;
    gameStarted = false;
    goto serverSelection;
  }

	printf("Color: %d, ", playerColor);
	printf("Won: %d\n", playerWon);

	while(!windowShouldClose()) {
		bool ret = drawEndScreen(playerWon, playerColor, &board);

		if(ret || playerWon == -1) {
      pthread_cancel(thread_NET);
      networkStatus = false;
      gameStarted = false;
      failedConnection = false;
      playerWon = NONE;
			goto serverSelection;
		}
	}
	
	closeWindow();
	pthread_cancel(thread_NET);
	close(sockfd);
	
	free(ip);

	return;
}
