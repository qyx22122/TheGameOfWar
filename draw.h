#include "raylib.h"

void initWindow(const char* title, int sizeX, int sizeY){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(sizeX, sizeY, title);
}
bool windowShouldClose(){
	return WindowShouldClose();
}

void drawBoard(Board* b){

	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int SIZE = min(screenWidth, screenHeight)/16;
	int offsetX = (screenWidth > screenHeight ? screenWidth : 0);
	int offsetY = ;

	BeginDrawing();
	{
		ClearBackground(RAYWHITE);

		for(int i = 0; i < 256; i++)
		{
			Color color = (b->green[i] ? GREEN : (b->blue[i] ? BLUE : WHITE));
			
			DrawRectangle(offsetX + (i % 16) * (SIZE + SPACING), offsetY + (i/16) * (SIZE + SPACING), SIZE, SIZE, color);
		}
	}
	EndDrawing();
}
