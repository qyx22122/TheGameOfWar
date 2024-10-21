#include "board.h"
#include "util.h"

#include <raylib.h>

#define MAX_IP_LENGHT 16
#define MAX_PORT_LENGHT 6

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
	
	int hoverIndex = (mousePos.x - offsetX)/size + ((int)(mousePos.y - offsetY) / (float)size)*BOARD_SIZE;

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

			Rectangle bounds = {(float)(offsetX + (i % BOARD_SIZE) * size), (float)(offsetY + ((float)i / BOARD_SIZE) * size), (float)size, (float)size};

			if(turn && onBoard && hoverIndex == i)
				color = playerColor ? SKYBLUE : LIME;
			if(turn && onBoard && !validHoverIndex && hoverIndex == i)
				color = RED;

			DrawRectangleRec(bounds, color);

			if(DEBUG_MODE && hoverIndex != i)
			{
				int c = count(b, i);
				DrawText(TextFormat("%d", c), bounds.x + bounds.width / 2 - (float)MeasureText(TextFormat("%d", c), 20) / 2, bounds.y + bounds.height / 2, 20, WHITE);
			}
			else if(DEBUG_MODE) {
				DrawText(TextFormat("%d", i), bounds.x + bounds.width / 2 - (float)MeasureText(TextFormat("%d", i), 20) / 2, bounds.y + bounds.height / 2, 20, WHITE);
			}
		}
	}
	EndDrawing();
	
	
	return move;
}

void drawLoading(char* ip, int port) {

	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();

	SetMouseCursor(MOUSE_CURSOR_DEFAULT);

	BeginDrawing();
	
	ClearBackground(DARKGRAY);

	DrawText("Waiting...", screenWidth/2 - MeasureText("Waiting...", 40) / 2, screenHeight/2 - 40, 40, LIME);
	const char* formattedText = TextFormat("Connecting to %s:%d", ip, port);
	DrawText(formattedText, screenWidth/2 - MeasureText(formattedText, 30) / 2, screenHeight/2 + 25, 30, SKYBLUE);

	EndDrawing();
}

bool drawEndScreen(int won, int playerColor, Board* b) {
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();

	int size = min(screenWidth, screenHeight)/BOARD_SIZE;
	int offsetX = screenWidth > screenHeight ? (screenWidth - screenHeight)/2 : 0;
	int offsetY = screenHeight > screenWidth ? (screenHeight - screenWidth)/2 : 0;
	
	BeginDrawing();
	
	for(int i = 0; i < BOARD_LENGHT; i++)
	{
		Color color = (b->green[i] ? GREEN : (b->blue[i] ? BLUE : BLACK));

		Rectangle bounds = {(float)(offsetX + (i % BOARD_SIZE) * size), (float)(offsetY + (i / BOARD_SIZE) * size), (float)size, (float)size};

		DrawRectangleRec(bounds, color);

		if(DEBUG_MODE)
			DrawText(TextFormat("%d", count(b, i)), bounds.x + bounds.width / 2, bounds.y + bounds.height / 2, 20, WHITE);
	}
	int width = size * BOARD_SIZE / 1.2;
	int height = size * BOARD_SIZE / 1.2;

	Rectangle endPopup = {(float)screenWidth / 2.0f - (float)width / 2.0f, (float)screenHeight / 2.0f - (float)height / 2.0f, (float)width, (float)height};
	
	DrawRectangleRec(endPopup, BLACK);
	DrawRectangleLinesEx(endPopup, 2.0, WHITE);

	const char* endText = won == 2 ? "Draw" : (won == playerColor ? "Victory" : "Lost");
	DrawText(endText, endPopup.x + endPopup.width / 2 - (float)MeasureText(endText, 50) / 2, endPopup.y + endPopup.height / 2, 50, won == 2 ? WHITE : (playerColor == 0 ? GREEN : BLUE));

	EndDrawing();

	return IsKeyPressed(KEY_ENTER);
}

bool drawServerSelection(char* ip, int* ipLenght, char* port, int* portLenght, bool* invalidAddress, bool failedConnection) {

	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();

	bool ipHover = false;
	bool portHover = false;
	bool connectHover = false;

	int ipWidth = MAX_IP_LENGHT * 35;
	int portWidth = MAX_PORT_LENGHT * 35;
	int height = 50;

	Rectangle ipBox = {(float)screenWidth / 2 - (float)ipWidth / 2, (float)screenHeight / 2 - (float)height / 2 - 35, (float)ipWidth, (float)height};
	Rectangle portBox = {screenWidth / 2.0f - portWidth / 2.0f, screenHeight / 2.0f - height / 2.0f + 35.0f, (float)portWidth, (float)height};
	Rectangle connectButton = {screenWidth / 2.0f - (float)MeasureText("Connect", 40) / 2 - 5, screenHeight / 2.0f + 90.0f, (float)MeasureText("Connect", 40) + 15, (float)height};

	if (CheckCollisionPointRec(GetMousePosition(), ipBox))
		ipHover = true;

	if (CheckCollisionPointRec(GetMousePosition(), portBox))
		portHover = true;

	if(CheckCollisionPointRec(GetMousePosition(), connectButton))
		connectHover = true;


	if(ipHover || portHover)
		SetMouseCursor(MOUSE_CURSOR_IBEAM);
	else
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);

	if(ipHover) {
		int key = GetCharPressed();
		// Check if more characters have been pressed on the same frame
		while (key > 0) {
			if (*ipLenght < MAX_IP_LENGHT - 1 && ((key >= 48 && key <= 57) || key == 46)) {
				ip[*ipLenght] = (char)key;
				ip[*ipLenght + 1] = '\0'; // Add null terminator at the end of the string.
				*ipLenght += 1;

				*invalidAddress = false;
			}
			key = GetCharPressed();  // Check next character in the queue
		}

		if (IsKeyPressed(KEY_BACKSPACE)) {
			*ipLenght -= 1;
			if (*ipLenght < 0)
				*ipLenght = 0;
			ip[*ipLenght] = '\0';
			*invalidAddress = false;
		}
	}
	if(portHover) {
		int key = GetCharPressed();
		// Check if more characters have been pressed on the same frame
		while (key > 0) {

			if (*portLenght < MAX_PORT_LENGHT - 1 && (key >= 48 && key <= 57)) {
				port[*portLenght] = (char)key;
				port[*portLenght + 1] = '\0'; // Add null terminator at the end of the string.
				*portLenght += 1;

				*invalidAddress = false;
			}
			key = GetCharPressed();  // Check next character in the queue
		}

		if (IsKeyPressed(KEY_BACKSPACE)) {
			*portLenght -= 1;
			if (*portLenght < 0)
				*portLenght = 0;
			port[*portLenght] = '\0';
			*invalidAddress = false;
		}
	}


	BeginDrawing();
	
	ClearBackground(DARKGRAY);

	if(failedConnection)
		DrawText("Failed to connect.", screenWidth / 2 - MeasureText("Failed to connect.", 30) / 2, screenHeight * 0.1, 30, RED);

	DrawRectangleRec(ipBox, *invalidAddress ? RED : LIGHTGRAY);
	DrawText(ip, (int)ipBox.x + 5, (int)ipBox.y + 8, 40, *invalidAddress ? WHITE : MAROON);

	DrawRectangleRec(portBox, *invalidAddress ? RED : LIGHTGRAY);
	DrawText(port, (int)portBox.x + 5, (int)portBox.y + 8, 40, *invalidAddress ? WHITE : MAROON);

	DrawRectangleRec(connectButton, MAROON);
	DrawText("Connect", (int)connectButton.x + 5, (int)connectButton.y + 8, 40, WHITE);
	
	if(ipHover) {
		DrawText("_", (int)ipBox.x + 8 + MeasureText(ip, 40), (int)ipBox.y + 12, 40, *invalidAddress ? WHITE : MAROON);
		DrawRectangleLinesEx(ipBox, 4.0, MAROON);
	}
	if(portHover) {
		DrawText("_", (int)portBox.x + 8 + MeasureText(port, 40), (int)portBox.y + 12, 40, *invalidAddress ? WHITE : MAROON);
		DrawRectangleLinesEx(portBox, 4.0, MAROON);
	}
	if(connectHover) {
		DrawRectangleLinesEx(connectButton, 3.0, WHITE);
	}

	EndDrawing();

	return IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && connectHover);
}
