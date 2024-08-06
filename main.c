#include "stdio.h"
#include "string.h"
#include "util.h"
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
void printBoard(Board* b) {
	for(int i = 0; i < 256; i++){
		printf("%c", (b->green[i] ? 'G' : (b->blue[i] ? 'B' : ' ')));
		if(i%16 == 15)
			printf("\n");
	}
	printf("----------------\n");
}
int count(Board* b, int i) {
	int count = 0;

	int x = i % 16;
	int y = i / 16;

	// Left
	if(x > 0) {
		count += b->green[x-1 + y * 16];
		count -= b->blue[x-1 + y * 16];
	}
	// Right
	if(x < 15) {
		count += b->green[x+1 + y * 16];
		count -= b->blue[x+1 + y * 16];
	}
	// Top
	if(y > 0) {
		count += b->green[x + (y-1) * 16];
		count -= b->blue[x + (y-1) * 16];
	}
	// Bottom
	if(y < 15) {
		count += b->green[x + (y+1) * 16];
		count -= b->blue[x + (y+1) * 16];
	}
	// Top Left
	if(x > 0 && y > 0) {
		count += b->green[x-1 + (y-1) * 16];
		count -= b->blue[x-1 + (y-1) * 16];
	}
	// Top Right
	if(x < 15 && y > 0) {
		count += b->green[x+1 + (y-1) * 16];
		count -= b->blue[x+1 + (y-1) * 16];
	}
	// Bottom Left
	if(x > 0 && y < 15) {
		count += b->green[x-1 + (y+1) * 16];
		count -= b->blue[x-1 + (y+1) * 16];
	}
	// Bottom Right
	if(x < 15 && y < 15) {
		count += b->green[x+1 + (y+1) * 16];
		count -= b->blue[x+1 + (y+1) * 16];
	}
	
	return count;
}


void updateBoard(Board* b){
	Board tmp;
	for(int i = 0; i < 256; i++) tmp.blue[i] = tmp.green[i] = false;
	int c;
	for(int i = 0; i < 256; i++){
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
	for(int i = 0; i < 256; i++){
		b->green[i] = tmp.green[i];
		b->blue[i] = tmp.blue[i];
	}
}

void initBoard(Board* b){
	for(int i = 0; i < 256; i++) b->blue[i] = b->green[i] = 0;
	char SP[] = "000000000000000001100110011001100110011001100110";
	for(int i = 0; i < strlen(SP); i++) b->blue[i] = SP[i] - '0';
	for(int i = 255; i > 255-strlen(SP); i--) b->green[i] = SP[255-i] - '0';
}


