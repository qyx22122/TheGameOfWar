#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "util.h"
#include "sp_util.h"
#include "board.h"
#include "draw.h"

#ifndef TGW_VERSION
#define TGW_VERSION "0.0.0.0"
#endif

void drawGUI();
void* connectToServer();

static Board board;
static MoveNotice turn;

static bool networkStatus;
static bool gameStarted;
static Move move;
static PlayerColor playerColor;
static pthread_t thread_NET;
static int playerWon;

static char* ip;
static int port;

static int sockfd;

static int counter;

int main() {
  drawGUI();
	return 0;
}
void* connectToServer(){

  memset(&board, 0, sizeof(Board));
	playerColor = PLAYER_NONE;
	gameStarted = false;
	playerWon = -1;
	turn = false;
	move.type = NONE;
  move.position = -1;

  networkStatus = true;

  if(initcs(&sockfd, ip, port) == -1) {
    networkStatus = false;
    return 0;
  }

	printf("Connected to server.\n");

	// Version Check
	char* version_buffer = TGW_VERSION;
	spsend(sockfd, version_buffer, strlen(version_buffer)+1);
	printf("Running version %s\n", TGW_VERSION);

	// Get player color from server
	sprecv(sockfd, &playerColor);

	if(playerColor == (int)OUTDATED_VERSION) {
		printf("Outdated client version.\n");
		networkStatus = false;
		return 0;
	}

	printf("Color: %s\n", playerColor == PLAYER_GREEN ? "Green" : "Blue");

	// Recive board update when game starts
	memset(&board, 0, sizeof(board));
	sprecv(sockfd, (void*)&board);
  printBoard(&board);

	gameStarted = true;

	while(gameStarted && networkStatus) {

		printf("Waiting for turn...\n");

		int ret = sprecv(sockfd, &turn);
		
		if(ret == 0) {
			printf("Server disconnected.\n");
			break;
		}
		
    printf("Turn: %d\n", turn);

		if(turn) {
			while(move.type == NONE) {}
      printf("Move: %d\n", move.position);
      printf("Counter: %d\n", counter);
      counter++;

			printf("Sending move.\n");
			spsend(sockfd, (void*)&move, sizeof(move));
			
			printf("Synchronizing with server.\n");
			MoveStatus moveStatus;
			ret = sprecv(sockfd, (void*)&moveStatus);

      printf("Error: %d\n", ret);

      printf("MS: %d\n", moveStatus);

			if(moveStatus == INVALID_MOVE) {
				printf("Invalid move sent.\n");
				
				// Recive updated board
        printf("Reciving board.\n");
				sprecv(sockfd, (void*)&board);
				continue;
			}
			
			printf("Move sent successfully.\n");
		}

		printf("Reciving updated board.\n");

		ret = sprecv(sockfd, (void*)&board);
		
    if(ret == 0) {
			printf("Server disconnected.\n");
			break;
		}
    
		GameState gameState;
		ret = sprecv(sockfd, &gameState);

		if(ret == 0) {
			printf("Server disconnected.\n");
			break;
		}

		printf("Game state: %d\n", gameState);

		if(gameState == CONTINUE)
			continue;
		else if(gameState == DRAW) {
			printf("Game Ended in a DRAW.\n");
			playerWon = 2;
			break;
		}
		else if(gameState == GREEN_WIN) {
			printf("GREEN won.\n");
			playerWon = 0;
			break;
		}
		else if(gameState == BLUE_WIN) {
			printf("BLUE won.\n");
			playerWon = 1;
			break;
		}
		else {
			printf("Server Error: Server spat out hot garbage.\n");
			break;
		}
	}

	gameStarted = false;
  networkStatus = false;
  pthread_exit(0);
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

  // To prevent connections to server after closing the window on the server selection screen
  if(windowShouldClose()) {
    goto close;
  }

	pthread_create(&thread_NET, NULL, &connectToServer, NULL);
  printf("Created network thread: %d\n", thread_NET);
  networkStatus = true;

	while(!windowShouldClose() && !gameStarted && networkStatus) {
		bool shouldStop = drawLoading(ip, port);
    if(shouldStop) {
      printf("Stopped waiting.\n");
      failedConnection = true;
      networkStatus = false;
      pthread_cancel(thread_NET);
      closes(sockfd);
      goto serverSelection;
    }
	}
  
	if(!networkStatus && !windowShouldClose()) {
    printf("Failed to connect!\n");
		failedConnection = true;
    networkStatus = false;
		goto serverSelection;
	}
  
  printf("Game has started.\n");

	while(!windowShouldClose() && networkStatus) {
    bool pressed = false;
		move = drawBoard(&board, turn, (int)playerColor, &pressed);
    if(pressed) {
      printf("Disconnected from game.\n");
      failedConnection = true;
      gameStarted = false;
      networkStatus = false;
      closes(sockfd);
      goto serverSelection;
    }
	}

  // Server probably closed mid game (other player disconnected)
  if(playerWon == -1 && !windowShouldClose()) {
    printf("Something exited.\n");
    gameStarted = false;
    failedConnection = true;
    closes(sockfd);
    goto serverSelection;
  }

	printf("Color: %d, ", playerColor);
	printf("Won: %d\n", playerWon);

	while(!windowShouldClose()) {
		bool ret = drawEndScreen(playerWon, playerColor, &board);

    // Go back to the server selection
		if(ret) {
      pthread_cancel(thread_NET);
      failedConnection = false;
      gameStarted = false;
      closes(sockfd);
			goto serverSelection;
		}
	}

close:
	
	closeWindow();
	closes(sockfd);
	
	free(ip);

	return;
}
