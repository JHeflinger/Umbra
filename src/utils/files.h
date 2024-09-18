#ifndef FILES_H
#define FILES_H

#include "easyobjects.h"
#include "raylib.h"

#define PATH_SIZE 1024

typedef struct {
    int active;
    Shader shader;
} SafeShader;

typedef struct {
    char raw[PATH_SIZE];
	int active;
    SafeShader shader;
} PathString;

DECLARE_ARRLIST(PathString);

void PopulateFilePaths(ARRLIST_PathString* paths, const char* extension, const char* dirpath);

#endif
