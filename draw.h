#include "raylib.h"

void initWindow(const char* title, int sizeX, int sizeY){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(sizeX, sizeY, title);
}
bool windowShouldClose(){
	return WindowShouldClose();
}

void drawBoard(){
	BeginDrawing();
	{
		ClearBackground(RAYWHITE);
		DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
	}
	EndDrawing();
}
