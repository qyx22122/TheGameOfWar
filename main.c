#include "stdio.h"
#include "util.h"

int count(Board b, int i, bool isGreen);
Board updateBoard(Board old);

int main(){
	printf("Hello World!\n");
}
int count(Board b, int i, bool isGreen){
	int count = 0;
	if(i > 8){
		if(i%8){
			count -= (b.green & i-9)? 1 : 0;
			count += (b.blue & i-9)? 1 : 0;
		}
		if(i%8 < 7){	
			count -= (b.green & i-7)? 1 : 0;
			count += (b.blue & i-7)? 1 : 0;
		}

		count -= (b.green & i-8)? 1 : 0;
		count += (b.blue & i-8)? 1 : 0;
	}
	if(i < (255-8)){
		if(i%8){
			count -= (b.green & i+9)? 1 : 0;
			count += (b.blue & i+9)? 1 : 0;
		}
		if(i%8 < 7){	
			count -= (b.green & i+7)? 1 : 0;
			count += (b.blue & i+7)? 1 : 0;
		}

		count -= (b.green & i+8)? 1 : 0;
		count += (b.blue & i+8)? 1 : 0;
	}
	if(i%8){
		count -= (b.green & i-1)? 1 : 0;
		count += (b.blue & i-1)? 1 : 0;
	}
	if(i%8 < 7){	
		count -= (b.green & i+1)? 1 : 0;
		count += (b.blue & i+1)? 1 : 0;
	}
	return count * (-1 * isGreen);
}





Board updateBoard(Board old){
	for(int i = 0; i < 256; i++)
		if(old.green & (1 << i))

