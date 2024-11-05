#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sp_util.h"
#include "util.h"
#include "board.h"

#ifndef TGW_VERSION
#define TGW_VERSION "0.0.0.0"
#endif

#ifndef TGW_PORT
#define TGW_PORT 42042
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

void handlePlayer(int connectionfd);
void createGame();

static Game game = {};
static bool gameStarted = false;
static bool gameEnded = false;
static PlayerColor turn = PLAYER_GREEN;

int main() {
	
  int sockfd = -1;
  if(initss(&sockfd, TGW_PORT) == -1) {
    perror("Failed to initialze server socket.\n");
    return 1;
  }
  printf("Running server %s\n", TGW_VERSION);
  printf("Running on port %d\n", TGW_PORT);

	createGame();
	printf("Created game.\n");
	printBoard(&game.board);

	printf("Server has started.\n");

	while(!gameStarted) {
    
    int newsockfd = -1;
    int ret = acceptss(sockfd, &newsockfd);
    if(ret == -1)
      return 1;

    if(ret == 1)
      continue;

		handlePlayer(newsockfd);
	}

	// Send board to both players
	if(!spsend(game.green.connectionfd, (void*)&game.board, sizeof(Board))) {
    printf("Couldn't send board to client.\n");
    gameEnded = true;
  }
	if(!spsend(game.blue.connectionfd, (void*)&game.board, sizeof(Board))) {
    printf("Couldn't send board to client.\n");
    gameEnded = true;
  }

	Move move;

	while(!gameEnded) {
		Player* turnPlayer = turn==PLAYER_GREEN? &game.green : &game.blue;
		int greenCfd = game.green.connectionfd;
		int blueCfd = game.blue.connectionfd;

		// Send move notice
		if(!ispsend(greenCfd, turn == PLAYER_GREEN)) {
			printf("Client disconnected.\n");
			printf("Terminating server.\n");
			break;
		}
		if(!ispsend(blueCfd, turn == PLAYER_BLUE)) {
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
	  if(!validMove(&game.board, &move, turn)) {
		  printf("Move isn't valid.\n");
      
      // Send error message to client
      if(!ispsend(turnPlayer->connectionfd, INVALID_MOVE)) {
        printf("Client disconnected.\n");
	  	  printf("Terminating server.\n");
        break;
      }
			
      // Send board to client
      if(!spsend(turnPlayer->connectionfd, (void*)&game.board, sizeof(Board))) {
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
		game.moveCount++;
		updateBoardMove(&game.board, &move, turn);

		// Send both players the updated board
		if(!spsend(greenCfd, (void*)&game.board, sizeof(Board))) {
      printf("Client disconnected.\n");
			printf("Terminating server.\n");
      break;
    }
		if(!spsend(blueCfd, (void*)&game.board, sizeof(Board))) {
      printf("Client disconnected.\n");
			printf("Terminating server.\n");
      break;
    }

		int winCondition = checkWinCondition(&game.board);
		
		if(winCondition == 2) {
			printf("Updated board sent to both players.\n");

			turn = turn == PLAYER_GREEN ? PLAYER_BLUE : PLAYER_GREEN;

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
		
		if(playerWon == PLAYER_GREEN) {
      // Error checking isn't necesarry as the game is over
			ispsend(greenCfd, GREEN_WIN);
			ispsend(blueCfd, GREEN_WIN);
		}
		else {
      // Error checking isn't necesarry as the game is over
			ispsend(greenCfd, BLUE_WIN);
			ispsend(blueCfd, BLUE_WIN);
		}

		break;

	}

	closes(game.green.connectionfd);
	closes(game.blue.connectionfd);
  closes(sockfd);

  main();

	return 0;
}

void createGame() {
	gameStarted = false;
	turn = PLAYER_GREEN;
	game.moveCount = 0;
	initBoard(&(game.board));
	game.green.connectionfd = -1;
	game.blue.connectionfd = -1;
}

void handlePlayer(int connectionfd) {
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
	if(game.green.connectionfd == -1) {
		printf("Client color: GREEN\n");
		ispsend(connectionfd, PLAYER_GREEN);

		game.green.connectionfd = connectionfd;
	}
	else if(game.blue.connectionfd == -1) {
		printf("Client color: BLUE\n");
		ispsend(connectionfd, PLAYER_BLUE);

		game.blue.connectionfd = connectionfd;
		gameStarted = true;
	}

}
