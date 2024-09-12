#include "core/editor.h"
#include "utils/files.h"

int main(int argc, char* argv[]) {
    
    ARRLIST_PathString arr = { 0 };
    PopulateFilePaths(&arr, ".c", ".");
    for (int i = 0; i < arr.size; i++)
        printf("%s\n", ARRLIST_PathString_get(&arr, i).raw);
    exit(0);

    RunEditor();
    return 0;
}