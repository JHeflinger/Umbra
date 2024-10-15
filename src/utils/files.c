#include "files.h"
#include "panels/console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include "platform/clean_windows.h"
#define PATH_SEPARATOR "\\"
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define PATH_SEPARATOR "/"
#endif

IMPL_ARRLIST(PathString);

void PopulateFilePaths(ARRLIST_PathString* paths, const char* extension, const char* alternate, const char* scene_ext, const char* dirpath) {
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
                PopulateFilePaths(paths, extension, alternate, scene_ext, path);
            } else {
                if (strstr(findFileData.cFileName, extension) != NULL || 
					strstr(findFileData.cFileName, alternate) != NULL ||
					strstr(findFileData.cFileName, scene_ext) != NULL) {
                    if (strcmp(findFileData.cFileName + strlen(findFileData.cFileName) - strlen(extension), extension) == 0 || 
						strcmp(findFileData.cFileName + strlen(findFileData.cFileName) - strlen(alternate), alternate) == 0 ||
						strcmp(findFileData.cFileName + strlen(findFileData.cFileName) - strlen(scene_ext), scene_ext) == 0) {
                        PathString ps = { 0 };
                        memcpy(ps.raw, path, PATH_SIZE);
						ps.alternate = strcmp(findFileData.cFileName + strlen(findFileData.cFileName) - strlen(alternate), alternate) == 0;
						ps.scenefile = strcmp(findFileData.cFileName + strlen(findFileData.cFileName) - strlen(scene_ext), scene_ext) == 0;
                        ARRLIST_PathString_add(paths, ps);
                    }
                }
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
    #else

    DIR* dir = opendir(dirpath);
    struct dirent* entry = NULL;
    struct stat statbuf = { 0 };
    char path[1024];
    while ((entry = readdir(dir)) != NULL) {
        snprintf(path, sizeof(path), "%s%s%s", dirpath, PATH_SEPARATOR, entry->d_name);
		stat(path, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                PopulateFilePaths(paths, extension, alternate, scene_ext, path);
            }
        } else if (S_ISREG(statbuf.st_mode)) {
            if (strstr(entry->d_name, extension) != NULL || 
				strstr(entry->d_name, alternate) != NULL ||
				strstr(entry->d_name, scene_ext) != NULL) {
                if (strcmp(entry->d_name + strlen(entry->d_name) - strlen(extension), extension) == 0 ||
					strcmp(entry->d_name + strlen(entry->d_name) - strlen(alternate), alternate) == 0 ||
					strcmp(entry->d_name + strlen(entry->d_name) - strlen(scene_ext), scene_ext) == 0) {
                    PathString ps = { 0 };
                    memcpy(ps.raw, path, PATH_SIZE);
					ps.alternate = strcmp(entry->d_name + strlen(entry->d_name) - strlen(alternate), alternate) == 0;
					ps.scenefile = strcmp(entry->d_name + strlen(entry->d_name) - strlen(scene_ext), scene_ext) == 0;
                    ARRLIST_PathString_add(paths, ps);
                }
            }
        }
    }
    closedir(dir);

    #endif
}

int SaveFile(const char* data, size_t datalen, const char* path) {
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
		char errbuff[1024];
        sprintf(errbuff, "Error opening file to save %s\n", path);
		ConsoleError(errbuff);
        return -1;
    }
    size_t written = fwrite(data, sizeof(char), datalen, file);
    if (written != datalen) {
		char errbuff[1024];
        sprintf(errbuff, "Error writing file to save %s\n", path);
		ConsoleError(errbuff);
        return -1;
    }
    fclose(file);
    return 0;
}
