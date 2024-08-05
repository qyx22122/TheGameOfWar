#include "stdbool.h"

#define SPACING 0

typedef struct Board{
	bool green[256];
	bool blue[256];
} Board;

int min(int x, int y) {
	return x < y ? x : y;
}
