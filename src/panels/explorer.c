#include "explorer.h"
#include "utils/files.h"
#include "raylib.h"

ARRLIST_PathString g_shader_paths = { 0 };

void InitializeExplorer() {
    PopulateFilePaths(&g_shader_paths, ".c", ".");
}

void DrawExplorer(float x, float y, float w, float h) {
	for (int i = 0; i < g_shader_paths.size; i++) {
		int ypos = y + 10 + (20*i);
		if (ypos < y + h)
			DrawText(ARRLIST_PathString_get(&g_shader_paths, i).raw, x + 10, ypos, 14, RAYWHITE);
	}
}
