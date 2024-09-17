#include "chain.h"
#include "raylib.h"
#include <string.h>

ARRLIST_PathString g_active_shaders = { 0 };

void UpdateChain(ARRLIST_PathString* paths) {
	for (int i = 0; i < paths->size; i++) {
		int found = 0;
		PathString str = ARRLIST_PathString_get(paths, i);
		if (str.active == 0) continue;
		for (int j = 0; j < g_active_shaders.size; j++) {
			PathString cmpstr = ARRLIST_PathString_get(&g_active_shaders, j);
			if (strcmp(cmpstr.raw, str.raw) == 0) {
				found = 1;
				break;
			}
		}
		if (found == 0)
			ARRLIST_PathString_add(&g_active_shaders, str);
	}
}

void DrawChain(float x, float y, float w, float h) {
	for (int i = 0; i < g_active_shaders.size; i++) {
		DrawText(ARRLIST_PathString_get(&g_active_shaders, i).raw, x + 10, y + 10 + (i*20), 14, RAYWHITE);
	}	
}
