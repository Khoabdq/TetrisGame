#include <raylib.h>

int main() {
	Color darkBlue = {44, 44, 127, 255};
	InitWindow(300, 600, "Raylib Tetris");
	SetTargetFPS(60);


	/*man hinh chinh cua chuong trinh*/
	while (WindowShouldClose() == false) {
		BeginDrawing();
		ClearBackground(darkBlue);

		EndDrawing();
	}

	CloseWindow();
}