#define NOB_IMPLEMENTATION
#include "lib\nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define RAYLIB_PATH "C:\\raylib\\raylib\\src" // Path to the Raylib source folder

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return -1;
    
    nob_cmd_append(&cmd, "cc", "-o", BUILD_FOLDER"main", SRC_FOLDER"main.c",
                "-I", ".\\lib",
                "-I", RAYLIB_PATH, "-L", RAYLIB_PATH, "-l:libraylib.a", "-lgdi32", "-lwinmm"); 
    
    // nob_cmd_append(&cmd, "cc", "-o", BUILD_FOLDER"main", SRC_FOLDER"main.c");

    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        nob_log(NOB_ERROR, "Compilation failed.");
        return 1;
    }

    nob_log(NOB_INFO, "Build successful.");
    
    nob_cmd_append(&cmd, "./build/main.exe");
    
    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        nob_log(NOB_ERROR, "Run failed.");
        return 1;
    } 
    
    return 0;
}