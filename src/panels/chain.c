#include "chain.h"
#include "utils/structs.h"
#include "raylib.h"
#include <string.h>

ARRLIST_PathString g_active_shaders = { 0 };
ARRLIST_size_t g_shader_chain_order = { 0 };
float g_chain_disposition = 0.0f;

void UpdateChain(ARRLIST_PathString* paths) {
	for (int i = 0; i < paths->size; i++) {
		int found = 0;
		PathString str = ARRLIST_PathString_get(paths, i);
		if (str.active == 1) {
			for (int j = 0; j < g_active_shaders.size; j++) {
				PathString cmpstr = ARRLIST_PathString_get(&g_active_shaders, j);
				if (strcmp(cmpstr.raw, str.raw) == 0) {
					found = 1;
					break;
				}
			}
			if (found == 0) {
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
			DrawText(ARRLIST_PathString_get(&g_active_shaders, ARRLIST_size_t_get(&g_shader_chain_order, i)).raw, xpos, ypos, 14, RAYWHITE);
		}
	}
}
