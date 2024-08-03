#include "stdio.h"
#include "util.h"
#include "draw.h"

int count(Board* board, int i);
void updateBoard(Board* board);

int main(){
	initWindow("The Game Of War", 800, 600);
	while(!windowShouldClose()){
		drawBoard();
	}
}
int count(Board* b, int i){
	int count = 0;
	if(i > 8){
		if(i%8){
			count -= (b->green & i-9)? 1 : 0;
			count += (b->blue & i-9)? 1 : 0;
		}
		if(i%8 < 7){	
			count -= (b->green & i-7)? 1 : 0;
			count += (b->blue & i-7)? 1 : 0;
		}

		count -= (b->green & i-8)? 1 : 0;
		count += (b->blue & i-8)? 1 : 0;
	}
	if(i < (255-8)){
		if(i%8){
			count -= (b->green & i+9)? 1 : 0;
			count += (b->blue & i+9)? 1 : 0;
		}
		if(i%8 < 7){	
			count -= (b->green & i+7)? 1 : 0;
			count += (b->blue & i+7)? 1 : 0;
		}

		count -= (b->green & i+8)? 1 : 0;
		count += (b->blue & i+8)? 1 : 0;
	}
	if(i%8){
		count -= (b->green & i-1)? 1 : 0;
		count += (b->blue & i-1)? 1 : 0;
	}
	if(i%8 < 7){	
		count -= (b->green & i+1)? 1 : 0;
		count += (b->blue & i+1)? 1 : 0;
	}
	return count;
}


void updateBoard(Board* b){
	Board tmp;
	tmp.green = 0;
	tmp.blue = 0;
	int c;
	for(int i = 0; i < 256; i++){
		c = count(b,i);
		if(b->green & (1 << i)){
			tmp.green ^= (c==2 || c==3)? 1<<i : 0;
			tmp.blue ^= (c<0)? 1<<i : 0;
		}else
			tmp.green ^= (c==3)? 1<<i : 0;
		c *= -1;
		if(b->blue & (1 << i)){
			tmp.blue ^= (c==2 || c==3)? 1<<i : 0;
			tmp.green ^= (c<0)? 1<<i : 0;
		}else
			tmp.blue ^= (c==3)? 1<<i : 0;
	}
	b->green = tmp.green;
	b->blue = tmp.blue;
}




