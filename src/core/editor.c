#include "editor.h"
#include "panels/viewport.h"
#include "raylib.h"
#include <stdio.h>

#define GRAY_0 (Color){ 30, 30, 30, 255 }
#define GRAY_1 (Color){ 50, 50, 50, 255 }
#define GRAY_2 (Color){ 70, 70, 70, 255 }
#define GRAY_X (Color){ 200, 200, 200, 255 }
#define LIGHT_BLUE (Color){ 100, 100, 200, 255 }

#define NAMEBAR_HEIGHT 20
#define MIN_VIEWPORT_WIDTH 200
#define MIN_CONFIG_WIDTH 200

#define DIVIDER_HOVERED(x, y, w, h) CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x - 5, y - 5, w + 10, h + 10 })
#define DRAW_DIVIDER(x, y, w, h) \
    DrawRectangle(g_central_divider, \
        0, 3, \
        GetScreenHeight(), \
        DIVIDER_HOVERED(x, y, w, h) || g_mouse_clicked_divider ? LIGHT_BLUE : GRAY_2);

int g_central_divider = 1200;
int g_mouse_clicked_divider = 0;
int g_focused_window = 1;

void DrawOverlay() {
    DrawRectangle(0, 0, GetScreenWidth(), NAMEBAR_HEIGHT, GRAY_1);
    DrawRectangle(0, NAMEBAR_HEIGHT, GetScreenWidth(), 3, GRAY_2);

    DrawText("Viewport", 4, 4, 16, g_focused_window == 0 ? YELLOW : GRAY_X);
    DrawText("Shader Chain", g_central_divider + 4 + 3, 4, 16, g_focused_window == 1 ? YELLOW : GRAY_X);

    DRAW_DIVIDER(g_central_divider, NAMEBAR_HEIGHT, 3, GetScreenHeight() - NAMEBAR_HEIGHT);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && DIVIDER_HOVERED(g_central_divider, NAMEBAR_HEIGHT, 3, GetScreenHeight() - NAMEBAR_HEIGHT)) g_mouse_clicked_divider = 1;
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) g_mouse_clicked_divider = 0;
    if (g_mouse_clicked_divider)
        g_central_divider += GetMouseDelta().x;
    g_central_divider = g_central_divider < MIN_CONFIG_WIDTH ? MIN_CONFIG_WIDTH : (g_central_divider > GetScreenWidth() - MIN_CONFIG_WIDTH ? GetScreenWidth() - MIN_CONFIG_WIDTH : g_central_divider);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 0, 0, g_central_divider, GetScreenHeight() }))
        g_focused_window = 0;
    else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ g_central_divider, 0, GetScreenWidth() - g_central_divider, GetScreenHeight() }))
        g_focused_window = 1;

    static float alpha = 0.0f;
    if (IsKeyDown(KEY_LEFT_ALT))
        alpha += 600.0f * GetFrameTime();
    else 
        alpha -= 900.0f * GetFrameTime();
    alpha = alpha < 0 ? 0 : (alpha > 255 ? 255 : alpha);
    char buffer[256];
    sprintf(buffer, "FPS: %d", (int)(1.0f/GetFrameTime()));
    DrawText(buffer, 10, 10, 18, (Color){ 255, 255, 255, alpha });
}

void DrawPanels() {
    RenderViewport(0, NAMEBAR_HEIGHT, g_central_divider, GetScreenHeight() - NAMEBAR_HEIGHT);
}

void RunEditor() {
	SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1600, 900, "Umbra");
    InitializeViewport();
    while(!WindowShouldClose()) {
        UpdateViewport();
        if (g_focused_window == 0) ViewportInput();
        BeginDrawing();
        ClearBackground(GRAY_0);
        DrawPanels();
        DrawOverlay();
        EndDrawing();
    }
    CleanViewport();
    CloseWindow();
}