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

void list_c_files(const char *dir_path);

#ifdef _WIN32
void list_c_files_windows(const char *dir_path) {
    printf("starting\n");
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(strcat(strdup(dir_path), "\\*"), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        perror("FindFirstFile");
        return;
    }

    do {
        printf("do\n");
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
            char path[MAX_PATH];
            snprintf(path, sizeof(path), "%s\\%s", dir_path, findFileData.cFileName);

            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                list_c_files_windows(path);
            } else {
                if (strstr(findFileData.cFileName, ".c") != NULL) {
                    printf("%s\n", path);
                }
            }
        }
        printf("end\n");
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}
#endif

void list_c_files(const char *dir_path) {
#ifdef _WIN32
    list_c_files_windows(dir_path);
#else
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char path[1024];

    if ((dir = opendir(dir_path)) == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        snprintf(path, sizeof(path), "%s%s%s", dir_path, PATH_SEPARATOR, entry->d_name);
        if (stat(path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                list_c_files(path);
            }
        } else if (S_ISREG(statbuf.st_mode)) {
            if (strstr(entry->d_name, ".c") != NULL) {
                printf("%s\n", path);
            }
        }
    }

    closedir(dir);
#endif
}


void test() {
    list_c_files(".");
    exit(0);
}