#include "raylib.h"

int main(int argc, char* argv[]) {
	SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(900, 600, "Hello World");
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){ 50, 50, 50, 255 });
        DrawText("Bello! (Minionese) World", 10, GetScreenHeight()/2, 18, RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
}