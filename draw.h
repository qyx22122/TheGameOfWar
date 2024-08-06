#include "raylib.h"

void initWindow(const char* title, int sizeX, int sizeY){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(sizeX, sizeY, title);
	SetTargetFPS(60);
}
bool windowShouldClose(){
	return WindowShouldClose();
}

extern int count(Board* b, int i);

bool lastTurn = false;

bool drawBoard(Board* b){

	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int size = min(screenWidth, screenHeight)/16;
	int offsetX = screenWidth > screenHeight ? (screenWidth - screenHeight)/2 : 0;
	int offsetY = screenHeight > screenWidth ? (screenHeight - screenWidth)/2 : 0;

	Vector2 mousePos = GetMousePosition();
	
	int hoverIndex = (mousePos.x - offsetX)/size + ((int)(mousePos.y -offsetY)/size)*16;
	
	bool validHoverIndex = 1;
	bool onBoard;


	if(hoverIndex >= 256 || hoverIndex < 0 ||(lastTurn)? b->green[hoverIndex] : b->blue[hoverIndex])
		validHoverIndex = 0;
	onBoard = (mousePos.x > offsetX && mousePos.x < screenWidth - offsetX);

	if(onBoard && validHoverIndex && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
		if(!lastTurn)
			b->green[hoverIndex] = !b->green[hoverIndex];
		else
			b->blue[hoverIndex] = !b->blue[hoverIndex];

		lastTurn = !lastTurn;
	}
		

	BeginDrawing();
	{
		ClearBackground(DARKGRAY);

		for(int i = 0; i < 256; i++)
		{
			Color color = (b->green[i] ? GREEN : (b->blue[i] ? BLUE : BLACK));

			Rectangle bounds = {offsetX + (i % 16) * size, offsetY + (i / 16) * size, size, size};

			if(onBoard && hoverIndex == i)
				color = lastTurn ? SKYBLUE : LIME;
			if(onBoard && !validHoverIndex && hoverIndex == i)
				color = RED;

			DrawRectangleRec(bounds, color);

			if(DEBUG_MODE == true)
				DrawText(TextFormat("%d", count(b, i)), bounds.x + bounds.width / 2, bounds.y + bounds.height / 2, 20, WHITE);
		}
	}
	EndDrawing();
	
	// Space
	if(IsKeyPressed(32)){
		lastTurn = !lastTurn;
		return true;
	}

	return false;
}
