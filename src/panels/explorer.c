#include "explorer.h"
#include "utils/colors.h"
#include "panels/chain.h"
#include "core/scene.h"
#include "raylib.h"

ARRLIST_PathString g_shader_paths = { 0 };
float g_explorer_disposition = 0.0f;

void InitializeExplorer() {
    PopulateFilePaths(&g_shader_paths, ".fs", ".vs", ".slumbra", ".");
}

void DrawExplorer(float x, float y, float w, float h) {
	if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, y, w, h })) {
		g_explorer_disposition += GetMouseWheelMoveV().y * 20.0f;
	}
	g_explorer_disposition = g_explorer_disposition > 0.0f ? 0.0f : (g_explorer_disposition < -20.0f * (g_shader_paths.size + 1) + h ? -20.0f * (g_shader_paths.size + 1) + h : g_explorer_disposition);
	if ((g_shader_paths.size + 1) * 20.0f < h) g_explorer_disposition = 0.0f;
	for (int i = 0; i < g_shader_paths.size; i++) {
		int ypos = y + 10 + (20*i) + g_explorer_disposition;
		int xpos = x + 10;
		if (ypos < y + h && ypos > -20.0f) {
			PathString ps = ARRLIST_PathString_get(&g_shader_paths, i);
			if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, ypos - 2, w, 18 })) {
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					if (ps.scenefile == 0) {
						g_shader_paths.data[i].active = !ps.active;
						UpdateChain(&g_shader_paths);
					} else {
						if (ps.active) {
							ResetScene();
							g_shader_paths.data[i].active = 0;
						} else {
							LoadSceneError err = LoadScene(ps.raw);
							if (err.type != NONE) {
								g_shader_paths.data[i].error_color = 255.0f;
							} else {
								for (int j = 0; j < g_shader_paths.size; j++)
									if (g_shader_paths.data[j].scenefile == 1)
										g_shader_paths.data[j].active = 0;
								g_shader_paths.data[i].active = 1;
							}
						}
					}
				}
				DrawRectangle(xpos - 5, ypos - 2, MeasureText(ps.raw, 14) + 4 + 30, 18, GRAY_2);
				xpos += 20;
				DrawRectangle(xpos - 20, ypos + 6, 12, 3, RAYWHITE);
				if (ps.active == 0)
					DrawRectangle(xpos - 16, ypos + 2, 3, 12, RAYWHITE);
			}
			if (ps.error_color > 0) {
				g_shader_paths.data[i].error_color -= 200.0f * GetFrameTime();
				g_shader_paths.data[i].error_color = g_shader_paths.data[i].error_color < 0 ? 0 : g_shader_paths.data[i].error_color;
			}
			DrawText(ps.raw, xpos, ypos, 14, ps.active == 1 ? (ps.scenefile == 1 ? GREEN : YELLOW) : (Color){ 255, 255 - ps.error_color, 255 - ps.error_color, 255 });
		}
	}
}
