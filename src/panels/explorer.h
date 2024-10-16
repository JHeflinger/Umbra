#ifndef EXPLORER_H
#define EXPLORER_H

#include "utils/files.h"

void InitializeExplorer();

void DrawExplorer(float x, float y, float w, float h);

int MatchPath(PathString* pathstr, const char* path);

#endif
