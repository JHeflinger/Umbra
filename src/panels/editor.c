#include "editor.h"
#include "raylib.h"
#include "easymemory.h"
#include <stdio.h>
#include <stdlib.h>

char* g_file_buffer = NULL;
size_t g_file_size = 0;

void DrawEditor(float x, float y, float w, float h) {
	DrawText("No file loaded!", x + w/2 - (MeasureText("No file loaded!", 14) / 2), y + 20, 14, RAYWHITE);
	DrawText("Right-click a file to load it!", x + w/2 - (MeasureText("Right-click a file to load it!", 14) / 2), y + 40, 14, RAYWHITE);
	if (g_file_buffer != NULL) {
		DrawText(g_file_buffer, x, y + 80, 14, RAYWHITE);
	}
}

int LoadEditorBuffer(const char* path) {
	if (g_file_buffer != NULL) EZFREE(g_file_buffer);
	FILE* file = fopen(path, "r");
    if (!file) {
        printf("Error opening file %s", path);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    g_file_size = ftell(file);
    rewind(file);
    g_file_buffer = EZALLOC(g_file_size + 1, sizeof(char));
    if (!g_file_buffer) {
        printf("Memory allocation failed\n");
        fclose(file);
        return -1;
    }
    size_t result = fread(g_file_buffer, 1, g_file_size, file);
    fclose(file);

    return 0;
}