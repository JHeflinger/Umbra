#include "editor.h"
#include "utils/colors.h"
#include "panels/viewport.h"
#include "panels/explorer.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

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
int g_tr_semi_focused = EXPLORER_FOCUSED;
int g_br_semi_focused = PROFILER_FOCUSED;

void DrawOverlay() {

    DrawRectangle(0, 0, GetScreenWidth(), NAMEBAR_HEIGHT, GRAY_1);
    DrawRectangle(0, NAMEBAR_HEIGHT, GetScreenWidth(), DIVIDER_WIDTH, GRAY_2);

    DrawRectangle(g_central_divider + DIVIDER_WIDTH, g_sub_divider + DIVIDER_WIDTH + NAMEBAR_HEIGHT, GetScreenWidth() - g_central_divider, NAMEBAR_HEIGHT, GRAY_1);
    DrawRectangle(g_central_divider + DIVIDER_WIDTH, g_sub_divider + DIVIDER_WIDTH + NAMEBAR_HEIGHT + NAMEBAR_HEIGHT, GetScreenWidth() - g_central_divider, DIVIDER_WIDTH, GRAY_2);
    
    DRAW_TAB(5, 5, 70, NAMEBAR_HEIGHT - 5, GRAY_2);
    if (g_tr_semi_focused == EXPLORER_FOCUSED) {
        DRAW_TAB(g_central_divider + 5, 5, 70, NAMEBAR_HEIGHT - 5, GRAY_2);
    } else if (g_tr_semi_focused == EDITOR_FOCUSED) {
        DRAW_TAB(g_central_divider + 5 + 70 + 5, 5, 60, NAMEBAR_HEIGHT - 5, GRAY_2);
    } else {
        printf("bad top-right focus value detected\n");
        exit(1);
    }
    if (g_br_semi_focused == PROFILER_FOCUSED) {
        DRAW_TAB(g_central_divider + 5, 5 + g_sub_divider + NAMEBAR_HEIGHT, 65, NAMEBAR_HEIGHT - 5, GRAY_2);
    } else if (g_br_semi_focused == CHAIN_FOCUSED) {
        DRAW_TAB(g_central_divider + 5 + 65 + 5, 5 + g_sub_divider + NAMEBAR_HEIGHT, 110, NAMEBAR_HEIGHT - 5, GRAY_2);
    } else {
        printf("bad bottom-right focus value detected\n");
        exit(1);
    }

    DrawText("Viewport", 10, 10, 14, g_focused_window == VIEWPORT_FOCUSED ? YELLOW : GRAY_X);
    DrawText("Explorer", g_central_divider + 10 + DIVIDER_WIDTH, 10, 14, g_focused_window == EXPLORER_FOCUSED ? YELLOW : GRAY_X);
    DrawText("Editor", g_central_divider + 10 + DIVIDER_WIDTH + 70 + 10, 10, 14, g_focused_window == EDITOR_FOCUSED ? YELLOW : GRAY_X);
    DrawText("Profiler", g_central_divider + 10 + DIVIDER_WIDTH, g_sub_divider + NAMEBAR_HEIGHT + 10, 14, g_focused_window == PROFILER_FOCUSED ? YELLOW : GRAY_X);
    DrawText("Shader Chain", g_central_divider + 10 + DIVIDER_WIDTH + 65 + 10, g_sub_divider + NAMEBAR_HEIGHT + 10, 14, g_focused_window == CHAIN_FOCUSED ? YELLOW : GRAY_X);

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

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 0, 0, g_central_divider, GetScreenHeight() })) {
        g_focused_window = VIEWPORT_FOCUSED;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ g_central_divider, NAMEBAR_HEIGHT, GetScreenWidth() - g_central_divider, g_sub_divider })) {
        if (g_tr_semi_focused == EXPLORER_FOCUSED)
            g_focused_window = EXPLORER_FOCUSED;
        else g_focused_window = EDITOR_FOCUSED;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ g_central_divider, NAMEBAR_HEIGHT + NAMEBAR_HEIGHT + g_sub_divider, GetScreenWidth() - g_central_divider, GetScreenHeight() - (NAMEBAR_HEIGHT + NAMEBAR_HEIGHT + g_sub_divider) })) {
        if (g_br_semi_focused == PROFILER_FOCUSED)
            g_focused_window = PROFILER_FOCUSED;
        else g_focused_window = CHAIN_FOCUSED;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ g_central_divider + 5, 5, 70, NAMEBAR_HEIGHT - 5 })) {
        g_tr_semi_focused = EXPLORER_FOCUSED;
        g_focused_window = EXPLORER_FOCUSED;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ g_central_divider + 5 + 70 + 5, 5, 60, NAMEBAR_HEIGHT - 5 })) {
        g_tr_semi_focused = EDITOR_FOCUSED;
        g_focused_window = EDITOR_FOCUSED;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ g_central_divider + 5, 5 + g_sub_divider + NAMEBAR_HEIGHT, 65, NAMEBAR_HEIGHT - 5 })) {
        g_br_semi_focused = PROFILER_FOCUSED;
        g_focused_window = PROFILER_FOCUSED;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ g_central_divider + 5 + 65 + 5, 5 + g_sub_divider + NAMEBAR_HEIGHT, 110, NAMEBAR_HEIGHT - 5 })) {
        g_br_semi_focused = CHAIN_FOCUSED;
        g_focused_window = CHAIN_FOCUSED;
    }

    static float alpha = 0.0f;
    if (IsKeyDown(KEY_LEFT_ALT))
        alpha += 600.0f * GetFrameTime();
    else 
        alpha -= 900.0f * GetFrameTime();
    alpha = alpha < 0 ? 0 : (alpha > 255 ? 255 : alpha);
    char buffer[256];
    sprintf(buffer, "FPS: %d", (int)(1.0f/GetFrameTime()));
    DrawText(buffer, 10, GetScreenHeight() - 20, 18, (Color){ 255, 255, 255, alpha });
}

void DrawPanels() {
    DrawViewport(0, NAMEBAR_HEIGHT, g_central_divider, GetScreenHeight() - NAMEBAR_HEIGHT);
    if (g_tr_semi_focused == EXPLORER_FOCUSED)
	    DrawExplorer(g_central_divider + DIVIDER_WIDTH, NAMEBAR_HEIGHT, GetScreenWidth() - g_central_divider - DIVIDER_WIDTH, g_sub_divider);
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
