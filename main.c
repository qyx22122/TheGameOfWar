#include "stdio.h"
#include "string.h"
#include "util.h"
#include "board.h"
#include "draw.h"

int count(Board* board, int i);
void updateBoard(Board* board);
void initBoard(Board* board);
void printBoard(Board* board);

int main(){
	Board b;
	initBoard(&b);

	bool turn = false;
	Move move;

	initWindow("The Game Of War", 800, 800);
	
	while(!windowShouldClose()) {
		
		int winCheck = checkWinCondition(&b);

		if(winCheck != 2) {
			bool reset = drawEndScreen(winCheck == -1 ? 2 : winCheck, winCheck, &b);
			if(reset) {
				initBoard(&b);
				turn = false;
			}
			continue;
		}

		move = drawBoard(&b, true, (PlayerColor)turn);
		
		if(move.type != NONE && validMove(&b, &move, turn)) {
			updateBoardMove(&b, &move, turn);
			turn = !turn;
		}
	}
}
