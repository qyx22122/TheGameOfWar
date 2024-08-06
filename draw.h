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

	for(int i = 0; i < 256; i++)
	{
		Rectangle bounds = {offsetX + (i % 16) * size, offsetY + (i / 16) * size, size, size};
		
		if(CheckCollisionPointRec(mousePos, bounds)) {

			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				if(!lastTurn)
					b->green[i] = !b->green[i];
				else
					b->blue[i] = !b->blue[i];

				lastTurn = !lastTurn;
			}
		}
	}

	BeginDrawing();
	{
		ClearBackground(DARKGRAY);

		for(int i = 0; i < 256; i++)
		{
			Color color = (b->green[i] ? GREEN : (b->blue[i] ? BLUE : BLACK));

			Rectangle bounds = {offsetX + (i % 16) * size, offsetY + (i / 16) * size, size, size};

			if (CheckCollisionPointRec(mousePos, bounds))
				color = lastTurn ? SKYBLUE : LIME;
			
			DrawRectangleRec(bounds, color);
		}
	}
	EndDrawing();
	
	// Space
	if(IsKeyPressed(32))
		return true;

	return false;
}
