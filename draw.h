#include "raylib.h"

void initWindow(const char* title, int sizeX, int sizeY){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(sizeX, sizeY, title);
}
bool windowShouldClose(){
	return WindowShouldClose();
}

bool lastTurn = false;

bool drawBoard(Board* b){

	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int size = min(screenWidth, screenHeight)/16;
	int offsetX = screenWidth > screenHeight ? (screenWidth - screenHeight)/2 : 0;
	int offsetY = screenHeight > screenWidth ? (screenHeight - screenWidth)/2 : 0;

	Vector2 mousePos = GetMousePosition();
	int hI = (mousePos.x - offsetX)/size + ((int)(mousePos.y -offsetY)/size)*16;
	bool validHI = 1;
	if(hI >= 256 || hI < 0 || (lastTurn)? b->green[hI] : b->blue[hI]) validHI = 0;
	if(validHI && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
		if(!lastTurn)
			b->green[hI] = !b->green[hI];
		else
			b->blue[hI] = !b->blue[hI];

		lastTurn = !lastTurn;
	}
		

	BeginDrawing();
	{
		ClearBackground(DARKGRAY);

		for(int i = 0; i < 256; i++)
		{
			Color color = (b->green[i] ? GREEN : (b->blue[i] ? BLUE : BLACK));

			Rectangle bounds = {offsetX + (i % 16) * size, offsetY + (i / 16) * size, size, size};

			if(hI == i)
				color = lastTurn ? SKYBLUE : LIME;
			if(!validHI && hI == i)
				color = RED;
			DrawRectangleRec(bounds, color);
		}
	}
	EndDrawing();
	
	// Space
	if(IsKeyPressed(32))
		return true;

	return false;
}
