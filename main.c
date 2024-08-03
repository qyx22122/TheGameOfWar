#include "stdio.h"

typedef unsigned _BitInt(256) u256;

typedef struct Board{
	u256 green;
	u256 blue;
} Board;

int countN(Board b,int i);
Board step(Board old);

int main(){
	printf("Hello World!\n");
}
int countG(Board b,int i){
	int count = 0;
	if(i > 8){
		if(i%8){
			count += (b.green & i-9)? 1 : 0;
			count -= (b.blue & i-9)? 1 : 0;
		}
		if(i%8 < 7){	
			count += (b.green & i-7)? 1 : 0;
			count -= (b.blue & i-7)? 1 : 0;
		}

		count += (b.green & i-8)? 1 : 0;
		count -= (b.blue & i-8)? 1 : 0;
	}
	if(i < (1<<256) - 8){
		if(i%8){
			count += (b.green & i+9)? 1 : 0;
			count -= (b.blue & i+9)? 1 : 0;
		}
		if(i%8 < 7){	
			count += (b.green & i+7)? 1 : 0;
			count -= (b.blue & i+7)? 1 : 0;
		}

		count += (b.green & i+8)? 1 : 0;
		count -= (b.blue & i+8)? 1 : 0;
	}
	if(i%8){
		count += (b.green & i-1)? 1 : 0;
		count -= (b.blue & i-1)? 1 : 0;
	}
	if(i%8 < 7){	
		count += (b.green & i+1)? 1 : 0;
		count -= (b.blue & i+1)? 1 : 0;
	}
	return count;
}





Board step(Board old){
	u256 x = 1;
	for(int i = 0; i < 256; i++)
		if(old.green & (x << 1))

