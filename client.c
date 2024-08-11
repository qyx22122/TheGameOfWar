#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/mman.h> // TODO : FIND A WINDOWS ALTERNITIVE
		      // TODO : qyx1 -> do your part
#include "sp_util.h"
#include "util.h"
#include "board.h"
#include "draw.h"

#define IP "127.0.0.1"
#define PORT 42042

#define VERSION "0.0.0.1"

void drawGUI();

static Board* board;
static bool* turn;
static bool* gameEnded;
static bool* networkStatus;
static bool* windowStatus;
static Move* move;
static PlayerColor* playerColor;

static int* sockfd;

int main() {
	
	// Map shared memory
	board = mmap(NULL, sizeof *board, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	turn = mmap(NULL, sizeof *turn, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	gameEnded = mmap(NULL, sizeof *gameEnded, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	move = mmap(NULL, sizeof *move, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	sockfd = mmap(NULL, sizeof *sockfd, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	networkStatus = mmap(NULL, sizeof *networkStatus, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	windowStatus = mmap(NULL, sizeof *networkStatus, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	playerColor = mmap(NULL, sizeof *playerColor, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	move->type = NONE;
	*gameEnded = false;
	*turn = false;
	*networkStatus = true;
	*windowStatus = true;
	*playerColor = NONE;

	// Launch child process for gui
	int pid = fork();
	if(pid == -1) {
		perror("Forked it. (Couldn't fork process for GUI)\n");
		return 1;
	}

	if(pid == 0) {
		drawGUI();
		close(*sockfd);
		*windowStatus = false;
		kill(pid, SIGTERM);
		kill(getpid(), SIGTERM);
	}

	*sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(*sockfd == -1) {
		perror("Couldn't create socket.\n");
		*networkStatus = false;
		return 1;
	}

	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(IP);

	printf("Connecting to %s:%d\n", IP, PORT);

	if(connect(*sockfd, (struct sockaddr*)&serverAddr, (socklen_t)sizeof(serverAddr)) == -1) {
		perror("Couldn't connect to server.\n");
		*networkStatus = false;
		return 1;
	}

	printf("Connected to server.\n");

	// Version Check
	char* version_buffer = VERSION;
	cspsend(*sockfd, version_buffer);
	printf("Running version %s\n", VERSION);

	// Get player color from server
	char* color;
	csprecv(*sockfd, &color);

	if(color[0] == 'E') {
		printf("Server Error -- %s\n", color);
		close(*sockfd);
		*networkStatus = false;
		return 1;
	}

	printf("Color: %s\n", color);

	if(color[0] == 'G') {
		*playerColor = 0;
	}
	else if(color[0] == 'B') {
		*playerColor = 1;
	}

	free(color);
	
	// Recive board update when game starts
	bzero(board, sizeof(*board));
	vsprecv(*sockfd, (void*)board);

	while(!*gameEnded && *windowStatus) {

		printf("Waiting for turn...\n");

		int ret = bsprecv(*sockfd, turn);
		
		printf("Turn: %d\n", *turn);

		if(ret == 0) {
			printf("Server disconnected.\n");
			break;
		}

		if(*turn) {
			while(move->type == NONE) {}

			printf("Sending move.\n");
			vspsend(*sockfd, (void*)move, sizeof(*move));
			
			printf("Synchronizing with server.\n");
			char* errorMsg;
			csprecv(*sockfd, &errorMsg);

			if(errorMsg[0] == 'E') {
				printf("Server Error -- %s\n", errorMsg);
				
				// Recive updated board
				vsprecv(*sockfd, (void*)board);
				continue;
			}
			
			if(errorMsg[0] != 'S') {
				printf("Error -- %s\n", errorMsg);
				break;
			}

			printf("Move sent successfully.\n");
		}

		printf("Reciving updated board.\n");

		vsprecv(*sockfd, (void*)board);
	}

	close(*sockfd);

	*networkStatus = false;

	return 0;
}

void drawGUI() {
	initWindow("The Game Of War", 800, 800);

	while(!windowShouldClose() && *networkStatus) {
		*move = drawBoard(board, *turn, (int)*playerColor);
	}
	
	closeWindow();

}
