#include "editor.h"
#include "raylib.h"

void DrawEditor(float x, float y, float w, float h) {
	DrawText("No file loaded!", x + w/2 - (MeasureText("No file loaded!", 14) / 2), y + 20, 14, RAYWHITE);
	DrawText("Right-click a file to load it!", x + w/2 - (MeasureText("Right-click a file to load it!", 14) / 2), y + 40, 14, RAYWHITE);
}
