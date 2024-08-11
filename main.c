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

	initWindow("The Game Of War", 800, 800);
	
	while(!windowShouldClose()) {
		bool update = drawBoard(&b);

		if(update)
			updateBoard(&b);
	}
}
