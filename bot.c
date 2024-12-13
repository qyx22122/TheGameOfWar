#include <stdlib.h>
#include <stdbool.h>
#include "board.h"
#include "draw.h"

#define MAX_DEPTH 3

Move root_move;

int count(Board* board, int i);
void advanceBoard(Board* board);
void initBoard(Board* board);
void printBoard(Board* board);

int generateMoves(Board* b, bool turn, Move* returnCont) { // -> returns number of moves

	Move m;
	int j = 0;
	for (int i = 0; i < BOARD_LENGHT; i++) {
		m.position = i;
		m.type = MOVE;
		if (validMove(b, &m, turn)/* && count(b, i) != 0*/) {
			returnCont[j] = m;
			j++;
		}
	}
	m.type = ADVANCE;
	returnCont[j] = m;

	return j+1;
}

int evaluate(Board* b, bool turn) {

	int cnt[2];
	cnt[0] = 0;
	cnt[1] = 0;
	for (int i = 0; i < BOARD_LENGHT; i++) {
		cnt[0] += b->green[i];
		cnt[1] += b->blue[i];
	}

	return min(cnt[turn], 25) - min(cnt[!turn], 25);
}

int search(Board* b, bool turn, int alpha, int beta, int depth) {

	int state = checkWinCondition(b);

	if (state != 2) { // game has ended
		if (state == -1) return 0; // draw
		else return (turn != state ? -1 : 1) * (999 - MAX_DEPTH + depth);
	}

	if (depth == 0)
		return evaluate(b, turn);

	int best_value = -1000;

	Move children[BOARD_LENGHT+1];
	int numOfMoves = generateMoves(b, turn, children);

	for (int i = 0; i < numOfMoves; i++) {
		Board newBoard;

		if (children[i].type != ADVANCE) 
			updateBoardMove(b, &children[i], turn);
		else {
			memcpy(&newBoard, b, sizeof(Board));	
			updateBoardMove(&newBoard, &children[i], turn);
		}

		int value = -search(children[i].type != ADVANCE ? b : &newBoard, !turn, -beta, -alpha, depth-1);	

		// Because we are ignoring advance moves we can bactrack this way
		if (children[i].type != ADVANCE)
			updateBoardMove(b, &children[i], turn);

		if (value > best_value) {

			if (depth == MAX_DEPTH)
				root_move = children[i];

			best_value = value;
			if (value > alpha) {
				alpha = value;
				if (value >= beta)
					return value;
			}
		}
	}

	return best_value;
}

int main() 
{
	Board b;
	initBoard(&b);

	bool bot = 1;
	bool turn = 0;
	bool exit = false;
	Move move;

	initWindow("The Game Of War", 800, 800);
	
	while ((!windowShouldClose()) && !exit) {
		
		int winCheck = checkWinCondition(&b);

		if(winCheck != 2) {
			bool reset = drawEndScreen(winCheck == -1 ? 2 : winCheck, winCheck, &b);
			if(reset) {
				initBoard(&b);
				turn = false;
			}
			continue;
		}

		if (turn == bot) {
			int eval = search(&b, turn, -1000, 1000, MAX_DEPTH);
			printf("Computer has evaluated position as %d and will play %d %d, heuristic: %d\n", eval, root_move.position, root_move.type, evaluate(&b, turn));
			move = root_move;
		} 
		else
			move = drawBoard(&b, true, (PlayerColor)turn, &exit);
		
		if (move.type != NONE && validMove(&b, &move, turn)) {
			updateBoardMove(&b, &move, turn);
			turn = !turn;
		}
	}

	closeWindow();
}
