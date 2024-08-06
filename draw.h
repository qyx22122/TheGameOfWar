#include "raylib.h"

void initWindow(const char* title, int sizeX, int sizeY){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(sizeX, sizeY, title);
}
bool windowShouldClose(){
	return WindowShouldClose();
}

bool drawBoard(Board* b){

	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int size = min(screenWidth, screenHeight)/16;
	int offsetX = screenWidth > screenHeight ? (screenWidth - screenHeight)/2 : 0;
	int offsetY = screenHeight > screenWidth ? (screenHeight - screenWidth)/2 : 0;

	BeginDrawing();
	{
		ClearBackground(DARKGRAY);

		for(int i = 0; i < 256; i++)
		{
			Color color = (b->green[i] ? GREEN : (b->blue[i] ? BLUE : BLACK));
			
			DrawRectangle(offsetX + (i % 16) * (size + SPACING), offsetY + (i/16) * (size + SPACING), size, size, color);
		}
	}
	EndDrawing();
	
	// Space
	if(IsKeyPressed(32))
		return true;

	return false;
}
