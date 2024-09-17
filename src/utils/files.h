#ifndef FILES_H
#define FILES_H

#include "easyobjects.h"

#define PATH_SIZE 1024

typedef struct {
    char raw[PATH_SIZE];
	int active;
} PathString;

DECLARE_ARRLIST(PathString);

void PopulateFilePaths(ARRLIST_PathString* paths, const char* extension, const char* dirpath);

#endif
