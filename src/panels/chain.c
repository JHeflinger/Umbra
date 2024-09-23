#include "chain.h"
#include "utils/structs.h"
#include "utils/colors.h"
#include "raylib.h"
#include <string.h>

ARRLIST_PathString g_active_shaders = { 0 };
ARRLIST_size_t g_shader_chain_order = { 0 };
float g_chain_disposition = 0.0f;

void UpdateChain(ARRLIST_PathString* paths) {
	for (int i = 0; i < paths->size; i++) {
		int found = 0;
		PathString str = ARRLIST_PathString_get(paths, i);
		if (str.scenefile == 1) continue;
		if (str.active == 1) {
			for (int j = 0; j < g_active_shaders.size; j++) {
				PathString cmpstr = ARRLIST_PathString_get(&g_active_shaders, j);
				if (strcmp(cmpstr.raw, str.raw) == 0) {
					found = 1;
					break;
				}
			}
			if (found == 0) {
				str.shader.active = 1;
				str.shader.shader = str.alternate ? LoadShader(str.raw, 0) : LoadShader(0, str.raw);
				ARRLIST_PathString_add(&g_active_shaders, str);
				ARRLIST_size_t_add(&g_shader_chain_order, g_active_shaders.size - 1);
			}
		} else {
			int removal = -1;
			for (int j = 0; j < g_active_shaders.size; j++) {
				PathString cmpstr = ARRLIST_PathString_get(&g_active_shaders, j);
				if (strcmp(cmpstr.raw, str.raw) == 0) {
					found = 1;
					removal = j;
					break;
				}
			}
			if (found == 1) {
				UnloadShader(ARRLIST_PathString_get(&g_active_shaders, removal).shader.shader);
				ARRLIST_PathString_remove(&g_active_shaders, removal);
				int order_ind = -1;
				for (int j = 0; j < g_shader_chain_order.size; j++) {
					size_t order = ARRLIST_size_t_get(&g_shader_chain_order, j);
					if (order == removal) order_ind = j;
					else if (order > removal) g_shader_chain_order.data[j] -= 1;
				}
				ARRLIST_size_t_remove(&g_shader_chain_order, order_ind);
			}
		}
	}
}

void DrawChain(float x, float y, float w, float h) {
	if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, y, w, h })) {
		g_chain_disposition += GetMouseWheelMoveV().y * 20.0f;
	}
	g_chain_disposition = g_chain_disposition > 0.0f ? 0.0f : (g_chain_disposition < -20.0f * (g_shader_chain_order.size + 1) + h ? -20.0f * (g_shader_chain_order.size + 1) + h : g_chain_disposition);
	if ((g_shader_chain_order.size + 1) * 20.0f < h) g_chain_disposition = 0.0f;
	for (int i = 0; i < g_shader_chain_order.size; i++) {
		int ypos = y + 10 + (20*i) + g_chain_disposition;
		int xpos = x + 10;
		if (ypos < y + h && ypos > y - 20.0f) {
			PathString ps = ARRLIST_PathString_get(&g_active_shaders, ARRLIST_size_t_get(&g_shader_chain_order, i));
			char buffer[2048];
			sprintf(buffer, "%d.) %s", i + 1, ps.raw);
			if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, ypos - 2, w, 18 })) {
				int modval = 55;
				if (i <= 0) modval -= 22;
				if (i >= g_shader_chain_order.size - 1) modval -= 23;
				DrawRectangle(xpos - 5, ypos - 2, MeasureText(buffer, 14) + 4 + modval, 18, GRAY_2);
				xpos += 45;
				int uphovered = 0;
				int downhovered = 0;
				if (i > 0 && CheckCollisionPointRec(GetMousePosition(), (Rectangle){xpos - 45, ypos, 14, 14 })) {
					uphovered = 1;
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						size_t temp = g_shader_chain_order.data[i];
						g_shader_chain_order.data[i] = g_shader_chain_order.data[i - 1];
						g_shader_chain_order.data[i - 1] = temp;
					}
				} else if (i < g_shader_chain_order.size - 1 && CheckCollisionPointRec(GetMousePosition(), i > 0 ? (Rectangle){ xpos - 24, ypos, 14, 14 } : (Rectangle){xpos - 45, ypos, 14, 14 })) {
					downhovered = 1;
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
						size_t temp = g_shader_chain_order.data[i];
						g_shader_chain_order.data[i] = g_shader_chain_order.data[i + 1];
						g_shader_chain_order.data[i + 1] = temp;
					}
				}
				if (i > 0)
					DrawTriangle((Vector2){ xpos - 31, ypos + 12 }, (Vector2){ xpos - 38, ypos + 2 }, (Vector2){ xpos - 45, ypos + 12 }, uphovered == 1 ? YELLOW : RAYWHITE);
				else xpos -= 22;
				if (i < g_shader_chain_order.size - 1)
					DrawTriangle((Vector2){ xpos - 24, ypos + 2 }, (Vector2){ xpos - 17, ypos + 12 }, (Vector2){ xpos - 10, ypos + 2 }, downhovered == 1 ? YELLOW : RAYWHITE);
				else xpos -= 23;
			}
			DrawText(buffer, xpos, ypos, 14, RAYWHITE);
		}
	}
}

size_t ShaderChainSize() {
	return g_active_shaders.size;
}

Shader GetShaderInChain(size_t index) {
	return ARRLIST_PathString_get(&g_active_shaders, ARRLIST_size_t_get(&g_shader_chain_order, index)).shader.shader;
}

void CleanChain() {
	for (int i = 0; i < g_active_shaders.size; i++) {
		UnloadShader(g_active_shaders.data[i].shader.shader);
	}
}
