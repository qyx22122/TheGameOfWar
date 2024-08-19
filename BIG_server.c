#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "sp_util.h"
#include "board.h"
#include "util.h"

#define VERSION "0.0.69"
#define PORT "42042"

typedef struct Player {
	int connectionfd;
	struct sockaddr_in addr;
} Player;

typedef struct Game{
	Board board;
	Player green;
	Player blue;
	int moveCount;
} Game;

void handlePlayer(int connectionfd, struct sockaddr_in addr);
int getMove(Player* player, Move* move);
void createGame();

static Game game = {};
static bool gameStarted = false;
static bool gameEnded = false;
static PlayerColor turn = GREEN;

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	int option = 1;
	
	if(sockfd == -1) {
		perror("Couldn't create socket.\n");
		return 1;
	}
	// Reuse address - stops adress already in use
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	struct sockaddr_in server = {AF_INET, htons(atoi(PORT))};
	
	if(bind(sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) == -1) {
		perror("Couldn't bind socket.\n");
		return 1;
	}

	if(listen(sockfd, 5) == -1) {
		perror("No ears. (Couldn't listen)\n");
		return 1;
	}
	
	createGame();
	printf("Created game.\n");
	printBoard(&game.board);

	printf("Server has started.\n");

	while(!gameStarted) {

		struct sockaddr_in clientAddr;
		socklen_t clientAddrLenght = sizeof(clientAddr);

		int newsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLenght);
		
		if(newsockfd == -1 && errno != 11) {
			perror("Couldn't accept the truth. (Accept failed)\n");
			return 1;
		}
		else if(newsockfd == -1)
			continue;

		if(&clientAddr == NULL) {
			perror("Couldn't get address.\n");
			return -1;
		}

		handlePlayer(newsockfd, clientAddr);
	}

	close(sockfd);
	// Send board to both players
	vspsend(game.green.connectionfd, (void*)&game.board, sizeof(Board));
	vspsend(game.blue.connectionfd, (void*)&game.board, sizeof(Board));

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

		int ret = getMove(turnPlayer, &move);

		if(ret == 0) {
			printf("Terminating server.\n");
			break;
		}

		// Invalid move
		if(ret == -1) {
			cspsend(turnPlayer->connectionfd, "E: INVALID MOVE");

			vspsend(turnPlayer->connectionfd, (void*)&game.board, sizeof(Board));

			continue;
		}

		cspsend(turnPlayer->connectionfd, "S");

		printf("Move: %d, ", move.type);
		printf("Move position: %d\n", move.position);
		
		// Apply the move on internal board
		game.moveCount++;
		updateBoardMove(&game.board, &move, turn);

		// Send both players the updated board
		vspsend(greenCfd, (void*)&game.board, sizeof(Board));
		vspsend(blueCfd, (void*)&game.board, sizeof(Board));

		int winCondition = checkWinCondition(&game.board);
		
		if(winCondition == 2) {
			printf("Updated board sent to both players.\n");

			turn = turn == GREEN ? BLUE : GREEN;

			cspsend(greenCfd, "C");
			cspsend(blueCfd, "C");
			continue;
		}
		else if(winCondition == -1) {
			printf("Game ended with a DRAW.\n");
			cspsend(greenCfd, "D");
			cspsend(blueCfd, "D");
			break;
		}
		
		PlayerColor playerWon = (PlayerColor)winCondition;
		
		if(playerWon == GREEN) {
			cspsend(greenCfd, "G");
			cspsend(blueCfd, "G");
		}
		else {
			cspsend(greenCfd, "B");
			cspsend(blueCfd, "B");
		}

		break;

	}

	close(game.green.connectionfd);
	close(game.blue.connectionfd);

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

void handlePlayer(int connectionfd, struct sockaddr_in addr) {
	printf("Client connected (%s:%d)\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	
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
		
		close(connectionfd);
		return;
	}

	free(clientVersion);
	
	// Asign color
	if(game.green.connectionfd == -1) {
		printf("Client color: GREEN\n");
		char* color = "G";
		cspsend(connectionfd, color);

		game.green.connectionfd = connectionfd;
		game.green.addr = addr;

	}
	else if(game.blue.connectionfd == -1) {
		printf("Client color: BLUE\n");
		char* color = "B";
		cspsend(connectionfd, color);

		game.blue.connectionfd = connectionfd;
		game.blue.addr = addr;

		gameStarted = true;
	}

}

int getMove(Player* player, Move* move) {

	if(vsprecv(player->connectionfd, (void*)move) <= 0) {
		printf("Client disconnected.\n");
		return 0;
	}

	// Check if move is valid
	if(move == NULL || !validMove(&game.board, move, turn)) {
		printf("Move isn't valid.\n");
		return -1;
	}

	printf("%s:%d: ", inet_ntoa(player->addr.sin_addr), ntohs(player->addr.sin_port));

	return 1;
}
