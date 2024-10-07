#include "editor.h"
#include "core/scene.h"
#include "utils/files.h"
#include "panels/chain.h"
#include "panels/explorer.h"
#include "raylib.h"
#include "easymemory.h"
#include <stdio.h>
#include <stdlib.h>

IMPL_ARRLIST(char);
IMPL_ARRLIST(Line);

char g_path[PATH_SIZE] = { 0 };
ARRLIST_Line g_buffer = { 0 };
float g_editor_disposition = 0.0f;
float g_horizontal_editor_disposition = 0.0f;
int g_cursor_line = 0;
int g_cursor_column = 5;
int g_editor_saved = 1;

void clean_buffer() {
	for (int i = 0; i < g_buffer.size; i++)
		ARRLIST_char_clear(&g_buffer.data[i].string);
	ARRLIST_Line_clear(&g_buffer);
}

void DrawEditor(float x, float y, float w, float h) {
	static float g_cursor_timer = 0.0f;
	static float g_cursor_timer_toggle = 0;
	if (g_path[0] == 0) {
		DrawText("No file loaded!", x + w/2 - (MeasureText("No file loaded!", 14) / 2), y + 20, 14, RAYWHITE);
		DrawText("Right-click a file to load it!", x + w/2 - (MeasureText("Right-click a file to load it!", 14) / 2), y + 40, 14, RAYWHITE);
	} else {
		if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, y, w, h }))
			g_editor_disposition += GetMouseWheelMoveV().y * 20.0f;
		g_editor_disposition = g_editor_disposition > 0.0f ? 0.0f : (g_editor_disposition < -20.0f * (g_buffer.size + 1) + h ? -20.0f * (g_buffer.size + 1) + h : g_editor_disposition);
		if ((g_buffer.size + 1) * 20.0f < h) g_editor_disposition = 0.0f;
		int xpos = x + 20 - g_horizontal_editor_disposition;
		size_t sudosize = g_buffer.size;
		while (sudosize > 0) {
			xpos += MeasureText("0", 14);
			sudosize /= 10;
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 mousecoords = GetMousePosition();
			if (CheckCollisionPointRec(mousecoords, (Rectangle){x, y, w, h})) {
				g_cursor_line = (mousecoords.y - y - g_editor_disposition - 10) / 20;
				char* interbuf = EZALLOC(g_buffer.data[g_cursor_line].string.size + 1, sizeof(char));
				g_cursor_column = g_buffer.data[g_cursor_line].string.size;
				for (int i = 0; i < g_buffer.data[g_cursor_line].string.size; i++) {
					interbuf[i] = g_buffer.data[g_cursor_line].string.data[i];
					char charbuf[2] = { 0 };
					float spos = xpos + MeasureTextEx(GetFontDefault(), interbuf, 14, 3).x;
					if (mousecoords.x <= spos) {
						charbuf[0] = interbuf[i];
						float midpoint = spos - (MeasureText(charbuf, 14)/2.0f);
						if (mousecoords.x <= midpoint)
							g_cursor_column = i;
						else
							g_cursor_column = i + 1;
						break;
					}
				}
				EZFREE(interbuf);
			}
		}
		if (IsKeyPressed(KEY_DOWN)) {
			g_cursor_timer_toggle = 1;
			g_cursor_timer = 0.5f;
			g_cursor_line = g_cursor_line < g_buffer.size - 1 ? g_cursor_line + 1 : g_cursor_line;
			if (((g_cursor_line + 2) * 20) + g_editor_disposition > h)
				g_editor_disposition = h - ((g_cursor_line + 2) * 20);
			if (g_cursor_column < 0) g_cursor_column = 0;
			if (g_cursor_column > g_buffer.data[g_cursor_line].string.size) g_cursor_column = g_buffer.data[g_cursor_line].string.size;
		}
		if (IsKeyPressed(KEY_UP)) {
			g_cursor_timer_toggle = 1;
			g_cursor_timer = 0.5f;
			g_cursor_line = g_cursor_line > 0 ? g_cursor_line - 1 : g_cursor_line;
			if ((g_cursor_line * 20) + g_editor_disposition < 0)
				g_editor_disposition = g_cursor_line * -20;
			if (g_cursor_column < 0) g_cursor_column = 0;
			if (g_cursor_column > g_buffer.data[g_cursor_line].string.size) g_cursor_column = g_buffer.data[g_cursor_line].string.size;
		}
		if (IsKeyPressed(KEY_RIGHT)) {
			g_cursor_timer_toggle = 1;
			g_cursor_timer = 0.5f;
			g_cursor_column++;
			if (g_cursor_column > g_buffer.data[g_cursor_line].string.size) g_cursor_column = g_buffer.data[g_cursor_line].string.size;
			char* rulerstr = EZALLOC(g_cursor_column + 1, sizeof(char));
			memcpy(rulerstr, g_buffer.data[g_cursor_line].string.data, g_cursor_column * sizeof(char));
			float xdif = MeasureTextEx(GetFontDefault(), rulerstr, 14, 3).x;
			EZFREE(rulerstr);
			if (xpos + xdif > x + w)
				g_horizontal_editor_disposition += (xpos + xdif) - (x + w);
		}
		if (IsKeyPressed(KEY_LEFT)) {
			g_cursor_timer_toggle = 1;
			g_cursor_timer = 0.5f;
			g_cursor_column--;
			if (g_cursor_column < 0) g_cursor_column = 0;
			char* rulerstr = EZALLOC(g_cursor_column + 1, sizeof(char));
			memcpy(rulerstr, g_buffer.data[g_cursor_line].string.data, g_cursor_column * sizeof(char));
			float xdif = MeasureTextEx(GetFontDefault(), rulerstr, 14, 3).x;
			EZFREE(rulerstr);
			if (xpos + xdif < xpos + g_horizontal_editor_disposition)
				g_horizontal_editor_disposition -= (xpos + g_horizontal_editor_disposition) - (xpos + xdif) - 3;
		}
		if (IsKeyPressed(KEY_BACKSPACE)) {
			if (g_buffer.data[g_cursor_line].string.size <= 0 && g_cursor_line > 0) {
				ARRLIST_char_clear(&g_buffer.data[g_cursor_line].string);
				ARRLIST_Line_remove(&g_buffer, g_cursor_line);
				g_cursor_line--;
				g_cursor_column = g_buffer.data[g_cursor_line].string.size;
				g_editor_saved = 0;
			} else if (g_buffer.data[g_cursor_line].string.size > 0) {
				if (g_cursor_column <= 0 && g_cursor_line > 0) {
					g_cursor_column = g_buffer.data[g_cursor_line - 1].string.size;
					for (int i = 0; i < g_buffer.data[g_cursor_line].string.size; i++)
						ARRLIST_char_add(&g_buffer.data[g_cursor_line - 1].string, g_buffer.data[g_cursor_line].string.data[i]);
					ARRLIST_char_clear(&g_buffer.data[g_cursor_line].string);
					ARRLIST_Line_remove(&g_buffer, g_cursor_line);
					g_cursor_line--;
					g_editor_saved = 0;
				} else if (g_cursor_column > 0) {
					ARRLIST_char_remove(&g_buffer.data[g_cursor_line].string, g_cursor_column - 1);
					g_buffer.data[g_cursor_line].string.data[g_buffer.data[g_cursor_line].string.size] = '\0';
					g_cursor_column--;
					g_editor_saved = 0;
				}	
			}
		}
		if (IsKeyPressed(KEY_DELETE)) {
			if (g_cursor_column == g_buffer.data[g_cursor_line].string.size && g_buffer.size > 0 && g_cursor_line < g_buffer.size - 1) {
				for (int i = 0; i < g_buffer.data[g_cursor_line + 1].string.size; i++)
					ARRLIST_char_add(&g_buffer.data[g_cursor_line].string, g_buffer.data[g_cursor_line + 1].string.data[i]);
				ARRLIST_char_clear(&g_buffer.data[g_cursor_line + 1].string);
				ARRLIST_Line_remove(&g_buffer, g_cursor_line + 1);
				g_editor_saved = 0;
			} else if (g_cursor_column != g_buffer.data[g_cursor_line].string.size) {
				ARRLIST_char_remove(&g_buffer.data[g_cursor_line].string, g_cursor_column);
				g_buffer.data[g_cursor_line].string.data[g_buffer.data[g_cursor_line].string.size] = '\0';
				g_editor_saved = 0;
			}
		}
		if (IsKeyPressed(KEY_ENTER)) {
			Line newstring = { 0 };
			for (int i = g_cursor_column; i < g_buffer.data[g_cursor_line].string.size; i++) {
				ARRLIST_char_add(&newstring.string, g_buffer.data[g_cursor_line].string.data[i]);
				g_buffer.data[g_cursor_line].string.data[i] = '\0';
			}
			g_buffer.data[g_cursor_line].string.size = g_cursor_column;
			ARRLIST_Line_add(&g_buffer, newstring);
			for (int i = g_buffer.size - 1; i > g_cursor_line; i--) {
				memcpy(&g_buffer.data[i], &g_buffer.data[i - 1], sizeof(Line));
			}
			memcpy(g_buffer.data + g_cursor_line + 1, &newstring, sizeof(Line));
			g_cursor_line++;
			g_cursor_column = 0;
			g_editor_saved = 0;
		}
		if (!IsKeyDown(KEY_LEFT_CONTROL)) {
			int c = '\0';
			while ((c = GetCharPressed()) != 0) {
				if (c >= 32 && c <= 126) {
					char curr = (char)c;
					ARRLIST_char_add(&g_buffer.data[g_cursor_line].string, curr);
					for (int i = g_buffer.data[g_cursor_line].string.size - 1; i > g_cursor_column; i--)
						g_buffer.data[g_cursor_line].string.data[i] = g_buffer.data[g_cursor_line].string.data[i - 1];
					g_buffer.data[g_cursor_line].string.data[g_cursor_column] = curr;
					g_cursor_column++;
					g_editor_saved = 0;
				}
			}
		} else if (IsKeyPressed(KEY_S)) {
			SaveEditorBuffer();	
		}
		for (int i = 0; i < g_buffer.size; i++) {
			int ypos = y + 10 + (i * 20) + g_editor_disposition;
			if (ypos >= y + h) break;
			if (i == g_cursor_line) {
				DrawRectangle(xpos - 10 + g_horizontal_editor_disposition, ypos - 3, w, 20, (Color){ 60, 60, 150, 255 });
			}
			if (g_buffer.data[i].string.size == g_buffer.data[i].string.maxsize) {
				ARRLIST_char_add(&g_buffer.data[i].string, '\0');
				ARRLIST_char_remove(&g_buffer.data[i].string, g_buffer.data[i].string.size - 1);
			}
			DrawTextEx(GetFontDefault(), g_buffer.data[i].string.data, (Vector2){xpos, ypos}, 14, 3, RAYWHITE);
			if (i == g_cursor_line) {
				g_cursor_timer = g_cursor_timer_toggle == 0 ? g_cursor_timer + GetFrameTime() : g_cursor_timer - GetFrameTime();
				if (g_cursor_timer > 0.5f) g_cursor_timer_toggle = 1;
				else if (g_cursor_timer < 0.0f) g_cursor_timer_toggle = 0;
				if (g_cursor_timer_toggle) {
					char* rulerstr = EZALLOC(g_cursor_column + 1, sizeof(char));
					memcpy(rulerstr, g_buffer.data[i].string.data, g_cursor_column * sizeof(char));
					float xdif = MeasureTextEx(GetFontDefault(), rulerstr, 14, 3).x;
					EZFREE(rulerstr);
					DrawRectangle(xpos + xdif, ypos - 3, 2, 20, RAYWHITE);
				}
			}
		}
		DrawRectangle(x, y, xpos - 10 - x + g_horizontal_editor_disposition, h, (Color){ 50, 50, 50, 255 });
		for (int i = 0; i < g_buffer.size; i++) {
			int ypos = y + 10 + (i * 20) + g_editor_disposition;
			if (ypos >= y + h) break;
			char buf[1024];
			sprintf(buf, "%d", i + 1);
			DrawText(buf, x + 5, ypos, 14, (Color){ 170, 170, 170, 255 });
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
	g_cursor_column = 0;
	g_editor_saved = 1;
    return 0;
}

int IsEditorSaved() {
	return g_editor_saved;
}

void SaveEditorBuffer() {
	FILE* file = fopen(g_path, "w");
	if (file == NULL) {
		printf("Unable to save file %s\n", g_path);
		return;
	}
	for (int i = 0; i < g_buffer.size; i++) {
		fprintf(file, "%s\n", g_buffer.data[i].string.data);
	}
	fclose(file);
	g_editor_saved = 1;

	PathString ps = { 0 };
	if (MatchPath(&ps, g_path) != 0) {
		printf("File does not exist in the explorer\n");
		return;
	}
	if (ps.scenefile) {
		Scene* scene;
		if ((scene = GetScene()) && strcmp(scene->path, ps.raw) == 0) {
			LoadSceneError err = LoadScene(ps.raw);
			if (err.type != NONE) {
				printf("error: %d on line %d\n", (int)err.type, (int)err.line);
				return;
			}
		}
	} else {
		PathString* active_shader;
		if ((active_shader = MatchPathInChain(ps.raw))) {
			printf("TODO: Refresh loaded shader\n");
		}
	}
}
