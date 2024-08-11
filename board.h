#define BOARD_SIZE 16
#define BOARD_LENGHT BOARD_SIZE*BOARD_SIZE

typedef struct Board {
	bool green[BOARD_LENGHT];
	bool blue[BOARD_LENGHT];
} Board;

typedef enum PlayerColor {
	GREEN = 0,
	BLUE = 1
} PlayerColor;

typedef enum MoveType {
	NONE = -1,
	ADVANCE = 0,
	MOVE = 1
} MoveType;

typedef struct Move {
	MoveType type;
	int position;
} Move;

bool validMove(Board* b, Move* move, int turn) {
	if(move->type == ADVANCE)
		return true;

	// Check if index is in bounds
	if(move->position < 0 || move->position >= BOARD_LENGHT) {
		return false;
	}
	
	if(turn == 0) {
		return !b->blue[move->position];
	}
	else {
		return !b->green[move->position];
	}

}

void initBoard(Board* b) {
	for(int i = 0; i < BOARD_LENGHT; i++) b->blue[i] = b->green[i] = 0;
	char SP[] = "000000000000000001100100001001100110111001110110000001000010000000000001100000000000000110000000";
	for(int i = 0; i < strlen(SP); i++) b->blue[i] = SP[i] - '0';
	for(int i = BOARD_LENGHT-1; i > BOARD_LENGHT-1-strlen(SP); i--) b->green[i] = SP[BOARD_LENGHT-1-i] - '0';
}

void printBoard(Board* b) {
	for(int i = 0; i < BOARD_LENGHT; i++){
		printf("%c", (b->green[i] ? 'G' : (b->blue[i] ? 'B' : ' ')));
		if(i%BOARD_SIZE == BOARD_SIZE-1)
			printf("\n");
	}
	printf("----------------\n");
}

int count(Board* b, int i) {
	int count = 0;

	int x = i % BOARD_SIZE;
	int y = i / BOARD_SIZE;

	// Left
	if(x > 0) {
		count += b->green[x-1 + y * BOARD_SIZE];
		count -= b->blue[x-1 + y * BOARD_SIZE];
	}
	// Right
	if(x < BOARD_SIZE-1) {
		count += b->green[x+1 + y * BOARD_SIZE];
		count -= b->blue[x+1 + y * BOARD_SIZE];
	}
	// Top
	if(y > 0) {
		count += b->green[x + (y-1) * BOARD_SIZE];
		count -= b->blue[x + (y-1) * BOARD_SIZE];
	}
	// Bottom
	if(y < BOARD_SIZE-1) {
		count += b->green[x + (y+1) * BOARD_SIZE];
		count -= b->blue[x + (y+1) * BOARD_SIZE];
	}
	// Top Left
	if(x > 0 && y > 0) {
		count += b->green[x-1 + (y-1) * BOARD_SIZE];
		count -= b->blue[x-1 + (y-1) * BOARD_SIZE];
	}
	// Top Right
	if(x < BOARD_SIZE-1 && y > 0) {
		count += b->green[x+1 + (y-1) * BOARD_SIZE];
		count -= b->blue[x+1 + (y-1) * BOARD_SIZE];
	}
	// Bottom Left
	if(x > 0 && y < BOARD_SIZE-1) {
		count += b->green[x-1 + (y+1) * BOARD_SIZE];
		count -= b->blue[x-1 + (y+1) * BOARD_SIZE];
	}
	// Bottom Right
	if(x < BOARD_SIZE-1 && y < BOARD_SIZE-1) {
		count += b->green[x+1 + (y+1) * BOARD_SIZE];
		count -= b->blue[x+1 + (y+1) * BOARD_SIZE];
	}
	
	return count;
}

void updateBoard(Board* b){
	Board tmp;
	for(int i = 0; i < BOARD_LENGHT; i++) tmp.blue[i] = tmp.green[i] = false;
	int c;
	for(int i = 0; i < BOARD_LENGHT; i++){
		c = count(b,i);
		if(b->green[i]) {
			if(c < 0) {
				tmp.green[i] = false;
				tmp.blue[i] = true;
			}
			if(c == 2 || c == 3) {
				tmp.green[i] = true;
			}
		}
		else {
			if(c == 3)
				tmp.green[i] = true;
		}
		
		c *= -1;

		if(b->blue[i]) {
			if(c < 0) {
				tmp.blue[i] = false;
				tmp.green[i] = true;
			}
			if(c == 2 || c == 3) {
				tmp.blue[i] = true;
			}
		}
		else {
			if(c == 3)
				tmp.blue[i] = true;
		}

	}
	for(int i = 0; i < BOARD_LENGHT; i++){
		b->green[i] = tmp.green[i];
		b->blue[i] = tmp.blue[i];
	}
}

void updateBoardMove(Board* b, Move* move, int turn) {
	if(!validMove(b, move, turn)) {
		printf("Cannot update board - Move isn't valid.\n");
		return;
	}

	if(move->type == ADVANCE) {
		updateBoard(b);
		return;
	}
	
	if(turn == 0) {
		b->green[move->position] = !b->green[move->position];
	}
	else {
		b->blue[move->position] = !b->blue[move->position];
	}
}
