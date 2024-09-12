#include "editor.h"
#include "panels/viewport.h"
#include "panels/explorer.h"
#include "raylib.h"
#include <stdio.h>

#define GRAY_0 (Color){ 30, 30, 30, 255 }
#define GRAY_1 (Color){ 40, 40, 40, 255 }
#define GRAY_2 (Color){ 100, 100, 100, 255 }
#define GRAY_X (Color){ 210, 210, 210, 255 }
#define LIGHT_BLUE (Color){ 100, 100, 200, 255 }

#define NAMEBAR_HEIGHT 25
#define MIN_VIEWPORT_WIDTH 200
#define MIN_CONFIG_WIDTH 200
#define DIVIDER_WIDTH 1

#define DIVIDER_HOVERED(x, y, w, h) CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x - 5, y - 5, w + 10, h + 10 })
#define DRAW_VERTICAL_DIVIDER() \
    DrawRectangle(DIVIDER_HOVERED(g_central_divider, NAMEBAR_HEIGHT, DIVIDER_WIDTH, GetScreenHeight() - NAMEBAR_HEIGHT) || g_mouse_clicked_divider ? g_central_divider - 1 : g_central_divider, \
        0, DIVIDER_HOVERED(g_central_divider, NAMEBAR_HEIGHT, DIVIDER_WIDTH, GetScreenHeight() - NAMEBAR_HEIGHT) || g_mouse_clicked_divider ? DIVIDER_WIDTH + 2 : DIVIDER_WIDTH, \
        GetScreenHeight(), \
        DIVIDER_HOVERED(g_central_divider, NAMEBAR_HEIGHT, DIVIDER_WIDTH, GetScreenHeight() - NAMEBAR_HEIGHT) || g_mouse_clicked_divider ? LIGHT_BLUE : GRAY_2);
#define DRAW_HORIZONTAL_DIVIDER() \
    DrawRectangle(g_central_divider + DIVIDER_WIDTH, \
        DIVIDER_HOVERED(g_central_divider + DIVIDER_WIDTH, NAMEBAR_HEIGHT + g_sub_divider, GetScreenWidth() - g_central_divider - DIVIDER_WIDTH, DIVIDER_WIDTH) || g_mouse_clicked_sub_divider ? \
            NAMEBAR_HEIGHT + g_sub_divider - 1 : NAMEBAR_HEIGHT + g_sub_divider, \
        GetScreenWidth() - g_central_divider - DIVIDER_WIDTH, \
        DIVIDER_HOVERED(g_central_divider + DIVIDER_WIDTH, NAMEBAR_HEIGHT + g_sub_divider, GetScreenWidth() - g_central_divider - DIVIDER_WIDTH, DIVIDER_WIDTH) || g_mouse_clicked_sub_divider ? DIVIDER_WIDTH + 2 : DIVIDER_WIDTH, \
        DIVIDER_HOVERED(g_central_divider + DIVIDER_WIDTH, NAMEBAR_HEIGHT + g_sub_divider, GetScreenWidth() - g_central_divider - DIVIDER_WIDTH, DIVIDER_WIDTH) || g_mouse_clicked_sub_divider ? LIGHT_BLUE : GRAY_2);
#define DRAW_TAB(x, y, w, h, c) { \
        DrawRectangleRounded((Rectangle){ x, y, w, h}, 0.5f, 20, c); \
        DrawRectangleRec((Rectangle){ x, y + (h)/2, w, (h)/2}, c); \
    }

typedef enum {
	VIEWPORT_FOCUSED = 0,
	EXPLORER_FOCUSED = 1,
    EDITOR_FOCUSED = 2,
    PROFILER_FOCUSED = 3,
    CHAIN_FOCUSED = 4
} FOCUSED_WINDOW;

int g_central_divider = 1200;
int g_sub_divider = 400;
int g_mouse_clicked_divider = 0;
int g_mouse_clicked_sub_divider = 0;
int g_focused_window = VIEWPORT_FOCUSED;

void DrawOverlay() {

    DrawRectangle(0, 0, GetScreenWidth(), NAMEBAR_HEIGHT, GRAY_1);
    DrawRectangle(0, NAMEBAR_HEIGHT, GetScreenWidth(), DIVIDER_WIDTH, GRAY_2);
    
    DRAW_TAB(5, 5, 70, NAMEBAR_HEIGHT - 5, GRAY_2);
    DRAW_TAB(g_central_divider + 5, 5, 70, NAMEBAR_HEIGHT - 5, GRAY_2);

    DrawText("Viewport", 10, 10, 14, g_focused_window == VIEWPORT_FOCUSED ? YELLOW : GRAY_X);
    DrawText("Explorer", g_central_divider + 10 + DIVIDER_WIDTH, 10, 14, g_focused_window == EXPLORER_FOCUSED ? YELLOW : GRAY_X);

    DRAW_VERTICAL_DIVIDER();
    DRAW_HORIZONTAL_DIVIDER();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && DIVIDER_HOVERED(g_central_divider, NAMEBAR_HEIGHT, DIVIDER_WIDTH, GetScreenHeight() - NAMEBAR_HEIGHT)) g_mouse_clicked_divider = 1;
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) g_mouse_clicked_divider = 0;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && DIVIDER_HOVERED(g_central_divider + DIVIDER_WIDTH, NAMEBAR_HEIGHT + g_sub_divider, GetScreenWidth() - g_central_divider - DIVIDER_WIDTH, DIVIDER_WIDTH)) g_mouse_clicked_sub_divider = 1;
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) g_mouse_clicked_sub_divider = 0;
    if (g_mouse_clicked_divider)
        g_central_divider += GetMouseDelta().x;
    if (g_mouse_clicked_sub_divider)
        g_sub_divider += GetMouseDelta().y;
    g_central_divider = g_central_divider < MIN_CONFIG_WIDTH ? MIN_CONFIG_WIDTH : (g_central_divider > GetScreenWidth() - MIN_CONFIG_WIDTH ? GetScreenWidth() - MIN_CONFIG_WIDTH : g_central_divider);
    g_sub_divider = g_sub_divider < 0 ? 0 : (g_sub_divider > GetScreenHeight() - NAMEBAR_HEIGHT ? GetScreenHeight() - NAMEBAR_HEIGHT : g_sub_divider);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 0, 0, g_central_divider, GetScreenHeight() }))
        g_focused_window = VIEWPORT_FOCUSED;
    else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ g_central_divider, 0, GetScreenWidth() - g_central_divider, GetScreenHeight() }))
        g_focused_window = EXPLORER_FOCUSED;

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
    DrawViewport(0, NAMEBAR_HEIGHT, g_central_divider, GetScreenHeight() - NAMEBAR_HEIGHT);
	DrawExplorer(g_central_divider + DIVIDER_WIDTH, NAMEBAR_HEIGHT, GetScreenWidth() - g_central_divider - DIVIDER_WIDTH, GetScreenHeight() - NAMEBAR_HEIGHT);
}

void RunEditor() {
	SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1600, 900, "Umbra");
    InitializeViewport();
	InitializeExplorer();
    while(!WindowShouldClose()) {
        UpdateViewport();
        if (g_focused_window == VIEWPORT_FOCUSED) ViewportInput();
        BeginDrawing();
        ClearBackground(GRAY_0);
        DrawPanels();
        DrawOverlay();
        EndDrawing();
    }
    CleanViewport();
    CloseWindow();
}
