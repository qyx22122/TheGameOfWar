#include "stdio.h"
#include "string.h"
#include "util.h"
#include "draw.h"

int count(Board* board, int i);
void updateBoard(Board* board);
void initBoard(Board* board);

int main(){
	//initWindow("The Game Of War", 800, 600);
	Board b;
	initBoard(&b);
	updateBoard(&b);
	for(int i = 0; i < 256; i++){
	       	printf("%c", (b.green[i] || b.blue[i])? '1' : ' ');
		if(i%16 == 15) printf("\n");
	}
	//while(!windowShouldClose()){
	//	drawBoard();
	//}
}
int count(Board* b, int i){
	int count = 0;
	if(i > 16){
		if(i%16){
			count += b->green[i-17];
			count -= b->blue[i-17];
		}
		if(i%16 < 15){	
			count += b->green[i-15];
			count -= b->blue[i-15];
		}

		count += b->green[i-16];
		count -= b->blue[i-16];
	}
	if(i < (255-16)){
		if(i%16){
			count += b->green[i+17];
			count -= b->blue[i+17];
		}
		if(i%16 < 15){	
			count += b->green[i+15];
			count -= b->blue[i+15];
		}

		count += b->green[i+16];
		count -= b->blue[i+16];
	}
	if(i%16){
		count += b->green[i-1];
		count -= b->blue[i-1];
	}
	if(i%16 < 15){	
		count += b->green[i+1];
		count -= b->blue[i+1];
	}
	return count;
}


void updateBoard(Board* b){
	Board tmp;
	for(int i = 0; i < 256; i++) tmp.blue[i] = tmp.green[i] = 0;
	int c;
	for(int i = 0; i < 256; i++){
		c = count(b,i);
		if(b->green[i]){
			tmp.green[i] = (c==2 || c==3);
			tmp.blue[i] = (c<0);
		}else
			tmp.green[i] = (c==3);
		c *= -1;
		if(b->blue[i]){
			tmp.blue[i] = (c==2 || c==3);
			tmp.green[i] = (c<0);
		}else
			tmp.blue[i] = (c==3);
	}
	for(int i = 0; i < 256; i++){
		b->green[i] = tmp.green[i];
		b->blue[i] = tmp.blue[i];
	}
}

void initBoard(Board* b){
	for(int i = 0; i < 256; i++) b->blue[i] = b->green[i] = 0;
	char SP[] = "000000000000000001100100001001100110111001110110000001000010000000000001100000000000000110000000";
	for(int i = 0; i < strlen(SP); i++) b->blue[i] = SP[i] - '0';
	for(int i = 255; i > 255-strlen(SP); i--) b->green[i] = SP[255-i] - '0';
}

