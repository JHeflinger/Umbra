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
	int alternate;
} PathString;

DECLARE_ARRLIST(PathString);

void PopulateFilePaths(ARRLIST_PathString* paths, const char* extension, const char* alternate, const char* dirpath);

int SaveFile(const char* data, size_t datalen, const char* path);

#endif
