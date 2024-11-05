#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "sp_util.h"
#include "util.h"
#include "board.h"

#ifndef TGW_VERSION
#define TGW_VERSION "0.0.0.0"
#endif

#ifndef TGW_PORT
#define TGW_PORT 42042
#endif

#ifndef TGW_THREADNUM
#define TGW_THREADNUM 10
#endif

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

static GameThread gameThreads[TGW_THREADNUM];

int main() {
	
  int sockfd = -1;
  if(initss(&sockfd, TGW_PORT) == -1) {
    perror("Failed to initialze server socket.\n");
    return 1;
  }
  printf("Running server %s with %d games.\n", TGW_VERSION, TGW_THREADNUM);
  printf("Running on port %d\n", TGW_PORT);
	printf("Server has started.\n");

  while(true) {
	  for (int i = 0; i < TGW_THREADNUM; i++){

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
	if(!spsend(gameThreads[i].game.green.connectionfd, (void*)&(gameThreads[i].game.board), sizeof(Board))) {
    printf("Couldn't send board to client.\n");
    gameThreads[i].gameEnded = true;
  }
	if(!spsend(gameThreads[i].game.blue.connectionfd, (void*)&(gameThreads[i].game.board), sizeof(Board))) {
    printf("Couldn't send board to client.\n");
    gameThreads[i].gameEnded = true;
  }
	Move move;

	while(!gameThreads[i].gameEnded) {
		Player* turnPlayer = gameThreads[i].turn==PLAYER_GREEN? &gameThreads[i].game.green : &gameThreads[i].game.blue;
		int greenCfd = gameThreads[i].game.green.connectionfd;
		int blueCfd = gameThreads[i].game.blue.connectionfd;

		// Send move notice
		if(!ispsend(greenCfd, gameThreads[i].turn==PLAYER_GREEN)) {
			printf("Client disconnected.\n");
			printf("Terminating server.\n");
			break;
		}
		if(!ispsend(blueCfd, gameThreads[i].turn==PLAYER_BLUE)) {
			printf("Client disconnected.\n");
			printf("Terminating server.\n");
			break;
		}
    
    // Recive move
    if(sprecv(turnPlayer->connectionfd, (void*)&move) <= 0) {
		  printf("Client disconnected.\n");
		  printf("Terminating server.\n");
      break;
	  }
	  // Check if move is valid
	  if(!validMove(&gameThreads[i].game.board, &move, gameThreads[i].turn)) {
		  printf("Move isn't valid.\n");
      
      // Send error message to client
      if(!ispsend(turnPlayer->connectionfd, INVALID_MOVE)) {
        printf("Client disconnected.\n");
	  	  printf("Terminating server.\n");
        break;
      }
			
      // Send board to client
      if(!spsend(turnPlayer->connectionfd, (void*)&gameThreads[i].game.board, sizeof(Board))) {
  		  printf("Client disconnected.\n");
	  	  printf("Terminating server.\n");
        break;
      }

			continue;
	  }

		if(!ispsend(turnPlayer->connectionfd, MOVE_SUCCESS)) {
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
		if(!spsend(greenCfd, (void*)&gameThreads[i].game.board, sizeof(Board))) {
      printf("Client disconnected.\n");
			printf("Terminating server.\n");
      break;
    }
		if(!spsend(blueCfd, (void*)&gameThreads[i].game.board, sizeof(Board))) {
      printf("Client disconnected.\n");
			printf("Terminating server.\n");
      break;
    }

		int winCondition = checkWinCondition(&gameThreads[i].game.board);
		
		if(winCondition == 2) {
			printf("Updated board sent to both players.\n");

			gameThreads[i].turn = gameThreads[i].turn == PLAYER_GREEN ? PLAYER_BLUE : PLAYER_GREEN;

			if(!ispsend(greenCfd, CONTINUE)) {
        printf("Client disconnected.\n");
        printf("Terminating server.\n");
        break;
      }
			
      if(!ispsend(blueCfd, CONTINUE)) {
  		  printf("Client disconnected.\n");
	  	  printf("Terminating server.\n");
        break;
      }

			continue;
		}
		else if(winCondition == -1) {
			printf("Game ended with a DRAW.\n");
		
      // Error checking isn't necesarry as the game is over
      ispsend(greenCfd, DRAW);
      ispsend(blueCfd, DRAW);

			break;
		}
		
		PlayerColor playerWon = (PlayerColor)winCondition;
		
    ispsend(greenCfd, playerWon);
    ispsend(blueCfd, playerWon);

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
	gameThreads[i].turn = PLAYER_GREEN;
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
	if(strcmp(clientVersion, TGW_VERSION)) {
		printf("Outdated client. Update to version %s\n", TGW_VERSION);

		ispsend(connectionfd, OUTDATED_VERSION);
		
		closes(connectionfd);
		return;
	}

	free(clientVersion);
	
	// Asign color
	if(gameThreads[i].game.green.connectionfd == -1) {
		printf("Client color: GREEN\n");
		ispsend(connectionfd, PLAYER_GREEN);

		gameThreads[i].game.green.connectionfd = connectionfd;
	}
	else if(gameThreads[i].game.blue.connectionfd == -1) {
		printf("Client color: BLUE\n");
		ispsend(connectionfd, PLAYER_BLUE);

		gameThreads[i].game.blue.connectionfd = connectionfd;
		gameThreads[i].gameStarted = true;
	}
}
