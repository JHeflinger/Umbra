#include "explorer.h"
#include "utils/files.h"

ARRLIST_PathString g_shader_paths = { 0 };

void InitializeExplorer() {
    PopulateFilePaths(&g_shader_paths, ".c", ".");
    for (int i = 0; i < g_shader_paths.size; i++)
        printf("%s\n", ARRLIST_PathString_get(&g_shader_paths, i).raw);
}

void DrawExplorer(float x, float y, float w, float h) {


}
