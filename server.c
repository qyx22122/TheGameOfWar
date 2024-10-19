#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sp_util.h"
#include "board.h"
#include "util.h"

#define VERSION "0.0.69"
#define PORT 42042

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
static PlayerColor turn = GREEN;

int main() {
	
  int sockfd = -1;
  if(initss(&sockfd, PORT) == -1) {
    perror("Failed to initialze server socket.\n");
    return 1;
  }

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

	closes(sockfd);

	// Send board to both players
	if(!vspsend(game.green.connectionfd, (void*)&game.board, sizeof(Board))) {
    printf("Couldn't send board to client.\n");
    gameEnded = true;
  }
	if(!vspsend(game.blue.connectionfd, (void*)&game.board, sizeof(Board))) {
    printf("Couldn't send board to client.\n");
    gameEnded = true;
  }

	Move move;

	while(!gameEnded) {
		Player* turnPlayer = turn==GREEN? &game.green : &game.blue;
		int greenCfd = game.green.connectionfd;
		int blueCfd = game.blue.connectionfd;

		// Send move notice
		if(!bspsend(greenCfd, turn==GREEN)) {
			printf("Client disconnected.\n");
			printf("Terminating server.\n");
			break;
		}
		if(!bspsend(blueCfd, turn==BLUE)) {
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
	  if(!validMove(&game.board, &move, turn)) {
		  printf("Move isn't valid.\n");
      
      // Send error message to client
      if(!cspsend(turnPlayer->connectionfd, "E: INVALID MOVE")) {
        printf("Client disconnected.\n");
	  	  printf("Terminating server.\n");
        break;
      }
			
      // Send board to client
      if(!vspsend(turnPlayer->connectionfd, (void*)&game.board, sizeof(Board))) {
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
		game.moveCount++;
		updateBoardMove(&game.board, &move, turn);

		// Send both players the updated board
		if(!vspsend(greenCfd, (void*)&game.board, sizeof(Board))) {
      printf("Client disconnected.\n");
			printf("Terminating server.\n");
      break;
    }
		if(!vspsend(blueCfd, (void*)&game.board, sizeof(Board))) {
      printf("Client disconnected.\n");
			printf("Terminating server.\n");
      break;
    }

		int winCondition = checkWinCondition(&game.board);
		
		if(winCondition == 2) {
			printf("Updated board sent to both players.\n");

			turn = turn == GREEN ? BLUE : GREEN;

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

	closes(game.green.connectionfd);
	closes(game.blue.connectionfd);
  closes(sockfd);

  main();

	return 0;
}

void createGame() {
	gameStarted = false;
	turn = GREEN;
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
	if(game.green.connectionfd == -1) {
		printf("Client color: GREEN\n");
		char* color = "G";
		cspsend(connectionfd, color);

		game.green.connectionfd = connectionfd;
	}
	else if(game.blue.connectionfd == -1) {
		printf("Client color: BLUE\n");
		char* color = "B";
		cspsend(connectionfd, color);

		game.blue.connectionfd = connectionfd;
		gameStarted = true;
	}

}
