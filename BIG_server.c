#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "sp_util.h"
#include "board.h"

#define VERSION "0.0.19.1"
#define PORT 42042
#define THREADNUM 10

typedef struct Player {
	int connectionfd;
} Player;

typedef struct Game{
	Board board;
	Player green;
	Player blue;
	int moveCount;
} Game;

typedef struct GameThread{
	Game game;
	bool gameStarted;
	bool gameEnded;
	PlayerColor turn;
	pthread_t thread;
} GameThread;

void handlePlayer(int connectionfd, int i);
void createGame(int i);
void* manageThread(void* pi);

static GameThread gameThreads[THREADNUM] = {};

int main() {
	
  int sockfd = -1;
  if(initss(&sockfd, PORT) == -1) {
    perror("Failed to initialze server socket.\n");
    return 1;
  }
	printf("Server has started.\n");
  while(true) {
	  for (int i = 0; i < THREADNUM; i++){

      if(gameThreads[i].gameStarted && !gameThreads[i].gameEnded)
        continue;

		  createGame(i);
		  printf("Created game %d.\n",i);
		  printBoard(&gameThreads[i].game.board);

		  while(!gameThreads[i].gameStarted){
			  int newsockfd = -1;
			  int ret = acceptss(sockfd, &newsockfd);
			  if(ret == -1) return 1;
			  if(ret == 1) break;

			  handlePlayer(newsockfd, i);
		  }

		  int i2 = i;
		  pthread_create(&gameThreads[i2].thread, NULL, &manageThread, (void*)&i2);

	  }
  }

	closes(sockfd);
  return 1;
}

void* manageThread(void* pi){
	int i = *(int*)pi;
	// Send board to both players
	if(!vspsend(gameThreads[i].game.green.connectionfd, (void*)&(gameThreads[i].game.board), sizeof(Board))) {
    printf("Couldn't send board to client.\n");
    gameThreads[i].gameEnded = true;
  }
	if(!vspsend(gameThreads[i].game.blue.connectionfd, (void*)&(gameThreads[i].game.board), sizeof(Board))) {
    printf("Couldn't send board to client.\n");
    gameThreads[i].gameEnded = true;
  }
	Move move;

	while(!gameThreads[i].gameEnded) {
		Player* turnPlayer = gameThreads[i].turn==GREEN? &gameThreads[i].game.green : &gameThreads[i].game.blue;
		int greenCfd = gameThreads[i].game.green.connectionfd;
		int blueCfd = gameThreads[i].game.blue.connectionfd;

		// Send move notice
		if(!bspsend(greenCfd, gameThreads[i].turn==GREEN)) {
			printf("Client disconnected.\n");
			printf("Terminating server.\n");
			break;
		}
		if(!bspsend(blueCfd, gameThreads[i].turn==BLUE)) {
			printf("Client disconnected.\n");
			printf("Terminating server.\n");
			break;
		}
    
    // Recive move
    if(vsprecv(turnPlayer->connectionfd, (void*)&move) <= 0) {
		  printf("Client disconnected.\n");
		  printf("Terminating server.\n");
      break;
	  }
	  // Check if move is valid
	  if(!validMove(&gameThreads[i].game.board, &move, gameThreads[i].turn)) {
		  printf("Move isn't valid.\n");
      
      // Send error message to client
      if(!cspsend(turnPlayer->connectionfd, "E: INVALID MOVE")) {
        printf("Client disconnected.\n");
	  	  printf("Terminating server.\n");
        break;
      }
			
      // Send board to client
      if(!vspsend(turnPlayer->connectionfd, (void*)&gameThreads[i].game.board, sizeof(Board))) {
  		  printf("Client disconnected.\n");
	  	  printf("Terminating server.\n");
        break;
      }

			continue;
	  }

		if(!cspsend(turnPlayer->connectionfd, "S")) {
      printf("Client disconnected.\n");
	  	printf("Terminating server.\n");
      break;
    }

		printf("Move: %d, ", move.type);
		printf("Move position: %d\n", move.position);
		
		// Apply the move on internal board
		gameThreads[i].game.moveCount++;
		updateBoardMove(&gameThreads[i].game.board, &move, gameThreads[i].turn);

		// Send both players the updated board
		if(!vspsend(greenCfd, (void*)&gameThreads[i].game.board, sizeof(Board))) {
      printf("Client disconnected.\n");
			printf("Terminating server.\n");
      break;
    }
		if(!vspsend(blueCfd, (void*)&gameThreads[i].game.board, sizeof(Board))) {
      printf("Client disconnected.\n");
			printf("Terminating server.\n");
      break;
    }

		int winCondition = checkWinCondition(&gameThreads[i].game.board);
		
		if(winCondition == 2) {
			printf("Updated board sent to both players.\n");

			gameThreads[i].turn = gameThreads[i].turn == GREEN ? BLUE : GREEN;

			if(!cspsend(greenCfd, "C")) {
        printf("Client disconnected.\n");
        printf("Terminating server.\n");
        break;
      }
			
      if(!cspsend(blueCfd, "C")) {
  		  printf("Client disconnected.\n");
	  	  printf("Terminating server.\n");
        break;
      }

			continue;
		}
		else if(winCondition == -1) {
			printf("Game ended with a DRAW.\n");
		
      // Error checking isn't necesarry as the game is over
      cspsend(greenCfd, "D");	
      cspsend(blueCfd, "D");

			break;
		}
		
		PlayerColor playerWon = (PlayerColor)winCondition;
		
		if(playerWon == GREEN) {
      // Error checking isn't necesarry as the game is over
			cspsend(greenCfd, "G");
			cspsend(blueCfd, "G");
		}
		else {
      // Error checking isn't necesarry as the game is over
			cspsend(greenCfd, "B");
			cspsend(blueCfd, "B");
		}

		break;

	}

	closew(gameThreads[i].game.green.connectionfd);
	closew(gameThreads[i].game.blue.connectionfd);
  gameThreads[i].gameStarted = false;
  gameThreads[i].gameEnded = false;

  return 0;
}

void createGame(int i) {
	gameThreads[i].gameStarted = false;
	gameThreads[i].turn = GREEN;
	gameThreads[i].game.moveCount = 0;
	initBoard(&(gameThreads[i].game.board));
	gameThreads[i].game.green.connectionfd = -1;
	gameThreads[i].game.blue.connectionfd = -1;
}

void handlePlayer(int connectionfd, int i) {
	printf("\n%d\n\n",connectionfd);
	char* clientVersion;
	csprecv(connectionfd, &clientVersion);
	printf("Client version: %s\n", clientVersion);
	
	// Version doesn't match
	if(strcmp(clientVersion, VERSION)) {
		printf("Outdated client. Update to version %s\n", VERSION);

		char errorMsg[13+strlen(VERSION)];
		strcpy(errorMsg, "E: OUTDATED: ");
		strcat(errorMsg, VERSION);

		cspsend(connectionfd, errorMsg);
		
		closes(connectionfd);
		return;
	}

	free(clientVersion);
	
	// Asign color
	if(gameThreads[i].game.green.connectionfd == -1) {
		printf("Client color: GREEN\n");
		char* color = "G";
		cspsend(connectionfd, color);

		gameThreads[i].game.green.connectionfd = connectionfd;
	}
	else if(gameThreads[i].game.blue.connectionfd == -1) {
		printf("Client color: BLUE\n");
		char* color = "B";
		cspsend(connectionfd, color);

		gameThreads[i].game.blue.connectionfd = connectionfd;
		gameThreads[i].gameStarted = true;
	}
}
