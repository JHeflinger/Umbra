#include "explorer.h"
#include "utils/colors.h"
#include "panels/chain.h"
#include "raylib.h"

ARRLIST_PathString g_shader_paths = { 0 };
float g_disposition = 0.0f;

void InitializeExplorer() {
    PopulateFilePaths(&g_shader_paths, ".c", ".");
}

void DrawExplorer(float x, float y, float w, float h) {
	if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, y, w, h })) {
		g_disposition += GetMouseWheelMoveV().y * 20.0f;
	}
	g_disposition = g_disposition > 0.0f ? 0.0f : (g_disposition < -20.0f * (g_shader_paths.size + 1) + h ? -20.0f * (g_shader_paths.size + 1) + h : g_disposition);
	if ((g_shader_paths.size + 1) * 20.0f < h) g_disposition = 0.0f;
	for (int i = 0; i < g_shader_paths.size; i++) {
		int ypos = y + 10 + (20*i) + g_disposition;
		int xpos = x + 10;
		if (ypos < y + h && ypos > -20.0f) {
			if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, ypos - 2, w, 18 })) {
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					g_shader_paths.data[i].active = !g_shader_paths.data[i].active;
					UpdateChain(&g_shader_paths);
				}
				DrawRectangle(xpos - 5, ypos - 2, MeasureText(ARRLIST_PathString_get(&g_shader_paths, i).raw, 14) + 4 + 30, 18, GRAY_2);
				xpos += 20;
				DrawRectangle(xpos - 20, ypos + 6, 12, 3, RAYWHITE);
				if (ARRLIST_PathString_get(&g_shader_paths, i).active == 0)
					DrawRectangle(xpos - 16, ypos + 2, 3, 12, RAYWHITE);
			}
			PathString ps = ARRLIST_PathString_get(&g_shader_paths, i);
			DrawText(ps.raw, xpos, ypos, 14, ps.active == 1 ? YELLOW : RAYWHITE);
		}
	}
}
