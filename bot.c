#include <stdlib.h>
#include <stdbool.h>
#include "board.h"
#include "draw.h"

#define INF 1000;

char tab[255];

int count(Board* board, int i);
void advanceBoard(Board* board);
void initBoard(Board* board);
void printBoard(Board* board);

typedef struct MoveEvalPair {
	Move move;
	int eval;
} MoveEvalPair;

char* tabit (int n) {
	memset(tab, ' ', 255);
	tab[2*n] = '\0';
	return tab;
}

int possibleMoves (Board* b, bool turn, Move* returnCont) { // -> returns number of moves
	Move m;
	int j = 0;
	for (int i = 0; i < BOARD_LENGHT; i++) {
		m.position = i;
		m.type = MOVE;
		if (validMove(b, &m, turn) && count(b, i) != 0) {
			returnCont[j] = m;
			j++;
		}
	}
	m.type = ADVANCE;
	returnCont[j] = m;
	return j+1;
}

int evaluatePosition (Board* b, bool turn) {
	int cnt[2];
	cnt[0] = 0;
	cnt[1] = 0;
	
	for (int i = 0; i < BOARD_LENGHT; i++) {
		cnt[0] += b->green[i];
		cnt[1] += b->blue[i];
	}

	return cnt[turn] - cnt[!turn];
}

MoveEvalPair chooseMoveBack (Board* b, bool turn, int toDepth, int depth) {
	// printf("%s%d: \n", tabit(2-depth), depth);
	int state = checkWinCondition(b);
	MoveEvalPair ret;

	if (state != 2) { // game has ended
		if (state == -1) ret.eval = 0; // draw
		else ret.eval = INF; // someone has won
		if (turn != state) ret.eval *= -1; // I have no idea why but it just works
		// printf("%s  Game has ended, eval: %d\n", tabit(2-depth), ret.eval);
	}
	else if (depth == toDepth) {
		// printf("%s  Reached the end of depth, eval: %d\n", tabit(2-depth), evaluatePosition(b, turn));
		ret.eval = evaluatePosition(b, turn);
	}
	else {
		MoveEvalPair best;
		best.eval = -INF;

		Move children[BOARD_LENGHT+1];
		int numOfMoves = possibleMoves(b, turn, children);
		// printf("%s  %d moves to explore\n", tabit(2-depth), numOfMoves);

		for (int i = 0; i < numOfMoves; i++) {
			Board newBoard;

			if (children[i].type != ADVANCE) updateBoardMove(b, &children[i], turn);
			else {
				memcpy(&newBoard, b, sizeof(Board));	
				updateBoardMove(&newBoard, &children[i], turn);
			}

			// printf("%s  Recursion for move no. %d\n", tabit(2-depth), i);
			int rec = -chooseMoveBack(children[i].type != ADVANCE ? b : &newBoard, !turn, toDepth, depth+1).eval;
			// printf("%sMove %d with type %d scored %d, Depth %d, Turn: %d\n", tabit(depth), children[i].position, children[i].type, rec, depth, turn);	
			

			if (rec > best.eval) {
				best.eval = rec;
				best.move = children[i];
				// printf("NEW BEST!\n");
			}

			if (children[i].type != ADVANCE) {
				// Because we are ignoring advance moves we can bactrack this way
				updateBoardMove(b, &children[i], turn);
			}
		}

		ret = best;
	}

	// printf("Score: %d, Depth %d, Turn: %d\n", ret.eval, depth, turn);

	// printf("%sReturning with move %d that has eval %d\n", tabit(2-depth), ret.move.position, ret.eval);	
	return ret;
}

MoveEvalPair chooseMoveFront (Board* b, bool turn, int toDepth) {
	return chooseMoveBack(b, turn, toDepth, 0);
}

int main(){
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
			MoveEvalPair engine = chooseMoveFront(&b, turn, 3);
			move = engine.move;
			printf("Computer has evaluated position as %d and will play %d %d, heuristic: %d\n", engine.eval, engine.move.position, engine.move.type, evaluatePosition(&b, turn));
		} else
			// move = chooseMove(&b, turn, 1).move;
			move = drawBoard(&b, true, (PlayerColor)turn, &exit);
		
		if (move.type != NONE && validMove(&b, &move, turn)) {
			updateBoardMove(&b, &move, turn);
			turn = !turn;
		}
	}

	closeWindow();
}
