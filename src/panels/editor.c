#include "editor.h"
#include "utils/files.h"
#include "raylib.h"
#include "easymemory.h"
#include <stdio.h>
#include <stdlib.h>

IMPL_ARRLIST(char);
IMPL_ARRLIST(Line);

char g_path[PATH_SIZE] = { 0 };
ARRLIST_Line g_buffer = { 0 };
float g_editor_disposition = 0.0f;
int g_cursor_line = 0;

void clean_buffer() {
	for (int i = 0; i < g_buffer.size; i++)
		ARRLIST_char_clear(&g_buffer.data[i].string);
	ARRLIST_Line_clear(&g_buffer);
}

void DrawEditor(float x, float y, float w, float h) {
	if (g_path[0] == 0) {
		DrawText("No file loaded!", x + w/2 - (MeasureText("No file loaded!", 14) / 2), y + 20, 14, RAYWHITE);
		DrawText("Right-click a file to load it!", x + w/2 - (MeasureText("Right-click a file to load it!", 14) / 2), y + 40, 14, RAYWHITE);
	} else {
		if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, y, w, h }))
			g_editor_disposition += GetMouseWheelMoveV().y * 20.0f;
		g_editor_disposition = g_editor_disposition > 0.0f ? 0.0f : (g_editor_disposition < -20.0f * (g_buffer.size + 1) + h ? -20.0f * (g_buffer.size + 1) + h : g_editor_disposition);

		if (IsKeyPressed(KEY_DOWN)) {
			g_cursor_line = g_cursor_line < g_buffer.size - 1 ? g_cursor_line + 1 : g_cursor_line;
			if (((g_cursor_line + 2) * 20) + g_editor_disposition > h)
				g_editor_disposition = h - ((g_cursor_line + 2) * 20);
		}

		if (IsKeyPressed(KEY_UP)) {
			g_cursor_line = g_cursor_line > 0 ? g_cursor_line - 1 : g_cursor_line;
			if ((g_cursor_line * 20) + g_editor_disposition < 0)
				g_editor_disposition = g_cursor_line * -20.0f;
		}

		int xpos = x + 20;
		size_t sudosize = g_buffer.size;
		while (sudosize > 0) {
			xpos += MeasureText("0", 14);
			sudosize /= 10;
		}
		DrawRectangle(x, y, xpos - 10 - x, h, (Color){ 50, 50, 50, 255 });
		for (int i = 0; i < g_buffer.size; i++) {
			int ypos = y + 10 + (i * 20) + g_editor_disposition;
			if (ypos >= y + h) break;
			char buf[1024];
			sprintf(buf, "%d", i + 1);
			DrawText(buf, x + 5, ypos, 14, (Color){ 170, 170, 170, 255 });
			if (i == g_cursor_line)
				DrawRectangle(xpos - 10, ypos - 3, w, 20, (Color){ 60, 60, 150, 255 });
			DrawText(g_buffer.data[i].string.data, xpos, ypos, 14, RAYWHITE);
		}
	}
}

int LoadEditorBuffer(const char* path) {
	FILE* file = fopen(path, "r");
    if (!file) {
        printf("Error opening file %s", path);
        return -1;
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    char* buffer = EZALLOC(file_size + 1, sizeof(char));
    if (!buffer) {
        printf("Memory allocation failed\n");
        fclose(file);
        return -1;
    }
    fread(buffer, 1, file_size, file);
    fclose(file);

	clean_buffer();
	size_t ind = 0;
	while (buffer[ind] != '\0') {
		if (buffer[ind] == '\n' || ind == 0) {
			Line line = { 0 };
			ARRLIST_Line_add(&g_buffer, line);
		}
		if (buffer[ind] != '\n'){
			ARRLIST_char_add(&g_buffer.data[g_buffer.size - 1].string, buffer[ind]);
		}
		ind++;
	}

	EZFREE(buffer);
	strcpy(g_path, path);
	g_editor_disposition = 0.0f;
	g_cursor_line = 0;
    return 0;
}
