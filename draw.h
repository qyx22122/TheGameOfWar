#include "raylib.h"

void initWindow(const char* title, int sizeX, int sizeY){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(sizeX, sizeY, title);
	SetTargetFPS(60);
}
bool windowShouldClose(){
	return WindowShouldClose();
}
void closeWindow() {
	CloseWindow();
}

extern int count(Board* b, int i);

Move drawBoard(Board* b, bool turn, int playerColor){

	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int size = min(screenWidth, screenHeight)/BOARD_SIZE;
	int offsetX = screenWidth > screenHeight ? (screenWidth - screenHeight)/2 : 0;
	int offsetY = screenHeight > screenWidth ? (screenHeight - screenWidth)/2 : 0;

	Vector2 mousePos = GetMousePosition();
	
	int hoverIndex = (mousePos.x - offsetX)/size + ((int)(mousePos.y -offsetY)/size)*BOARD_SIZE;

	bool validHoverIndex = 1;
	bool onBoard = (mousePos.x > offsetX && mousePos.x < screenWidth - offsetX);
	Move move;
	move.type = NONE;
	move.position = -1;

	if(playerColor == -1) {
		turn = false;
	}

	if(hoverIndex >= BOARD_LENGHT || hoverIndex < 0 || (playerColor == 0 ? b->blue[hoverIndex] : b->green[hoverIndex]))
		validHoverIndex = 0;

	if(turn && onBoard && validHoverIndex && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		move.type = MOVE;
		move.position = hoverIndex;
	}
	else if(turn && IsKeyPressed(32)){
		// Space
		move.type = ADVANCE;
	}

	BeginDrawing();
	{
		ClearBackground(DARKGRAY);

		for(int i = 0; i < BOARD_LENGHT; i++)
		{
			Color color = (b->green[i] ? GREEN : (b->blue[i] ? BLUE : BLACK));
			color = turn ? color : (b->green[i] ? GREEN : (b->blue[i] ? BLUE : BLACK));

			Rectangle bounds = {offsetX + (i % BOARD_SIZE) * size, offsetY + (i / BOARD_SIZE) * size, size, size};

			if(turn && onBoard && hoverIndex == i)
				color = playerColor ? SKYBLUE : LIME;
			if(turn && onBoard && !validHoverIndex && hoverIndex == i)
				color = RED;

			DrawRectangleRec(bounds, color);

			if(DEBUG_MODE == true)
				DrawText(TextFormat("%d", count(b, i)), bounds.x + bounds.width / 2, bounds.y + bounds.height / 2, 20, WHITE);
		}
	}
	EndDrawing();
	
	
	return move;
}
