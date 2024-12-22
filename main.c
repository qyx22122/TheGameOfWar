#include <stdbool.h>
#include "board.h"
#include "draw.h"


int main(){
	Board b;
	initBoard(&b);

	bool turn = false;
  bool exit = false;
	Move move;

	initWindow("The Game Of War", 800, 800);
	
	while((!windowShouldClose()) && !exit) {
		
		int winCheck = checkWinCondition(&b);

		if(winCheck != 2) {
			bool reset = drawEndScreen(winCheck == -1 ? 2 : winCheck, winCheck, &b);
			if(reset) {
				initBoard(&b);
				turn = false;
			}
			continue;
		}

		move = drawBoard(&b, true, (PlayerColor)turn, &exit);
		
		if(move.type != NONE && validMove(&b, &move, turn)) {
			updateBoardMove(&b, &move, turn);
			turn = !turn;
		}
	}

  closeWindow();
}
