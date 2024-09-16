#include "explorer.h"
#include "utils/files.h"
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
		if (ypos < y + h)
			DrawText(ARRLIST_PathString_get(&g_shader_paths, i).raw, x + 10, ypos, 14, RAYWHITE);
	}
}
