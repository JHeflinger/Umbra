#ifndef EDITOR_H
#define EDITOR_H

#include "easyobjects.h"

DECLARE_ARRLIST(char);

typedef struct {
	ARRLIST_char string;
} Line;

DECLARE_ARRLIST(Line);

void DrawEditor(float x, float y, float w, float h);

int LoadEditorBuffer(const char* path);

#endif
