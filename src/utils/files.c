#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEPARATOR "\\"
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define PATH_SEPARATOR "/"
#endif

IMPL_ARRLIST(PathString);

void PopulateFilePaths(ARRLIST_PathString* paths, const char* extension, const char* dirpath) {
    #ifdef _WIN32

    WIN32_FIND_DATA findFileData;
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", dirpath);
    HANDLE hFind = FindFirstFile(search_path, &findFileData);
    do {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
            char path[MAX_PATH];
            snprintf(path, sizeof(path), "%s\\%s", dirpath, findFileData.cFileName);
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                PopulateFilePaths(paths, extension, path);
            } else {
                if (strstr(findFileData.cFileName, extension) != NULL) {
                    if (strcmp(findFileData.cFileName + strlen(findFileData.cFileName) - strlen(extension), extension) == 0) {
                        PathString ps = { 0 };
                        memcpy(ps.raw, path, PATH_SIZE);
                        ARRLIST_PathString_add(paths, ps);
                    }
                }
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);

    #else

    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char path[1024];
    while ((entry = readdir(dir)) != NULL) {
        snprintf(path, sizeof(path), "%s%s%s", dirpath, PATH_SEPARATOR, entry->d_name);
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                PopulateFilePaths(paths, extension, path);
            }
        } else if (S_ISREG(statbuf.st_mode)) {
            if (strstr(entry->d_name, extension) != NULL) {
                if (strcmp(entry->d_name + strlen(entry->d_name) - strlen(extension), extension) == 0) {
                    PathString ps = { 0 };
                    memcpy(ps.raw, path, PATH_SIZE);
                    ARRLIST_PathString_add(paths, ps);
                }
            }
        }
    }
    closedir(dir);

    #endif
}