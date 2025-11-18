#define NOB_IMPLEMENTATION
#include "lib/nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define RAYLIB_PATH "C:\\raylib\\raylib\\src" // Path to the Raylib source folder (Windows)

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};
    int system;
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return -1;
    
    #ifdef __linux__ 
        nob_cmd_append(&cmd, "cc", "-o", BUILD_FOLDER"main", SRC_FOLDER"main.c");
        // nob_cmd_append(&cmd,"-Wall", "-Wextra", "-Werror");
        nob_cmd_append(&cmd,"-lraylib", "-lGL", "-lm", "-lpthread", "-ldl", "-lrt", "-lX11");
    #elif _WIN32
        nob_cmd_append(&cmd, "cc", "-o", BUILD_FOLDER"main", SRC_FOLDER"main.c",
                    "-I", ".\\lib",
                    "-I", RAYLIB_PATH, "-L", RAYLIB_PATH, "-l:libraylib.a", "-lgdi32", "-lwinmm"); 
    #else
        nob_log(NOB_ERROR, "Compilation target not supported.");
        return 1;
    #endif
    
    // nob_cmd_append(&cmd, "cc", "-o", BUILD_FOLDER"main", SRC_FOLDER"main.c");

    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        nob_log(NOB_ERROR, "Compilation failed.");
        return 1;
    }

    nob_log(NOB_INFO, "Build successful.");
    
    #ifdef __linux__ 
        nob_cmd_append(&cmd, "./build/main");
    #elif _WIN32
        nob_cmd_append(&cmd, "./build/main.exe");
    #else
        nob_log(NOB_ERROR, "OS not supported.");
        return 1;
    #endif

    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        nob_log(NOB_ERROR, "Run failed.");
        return 1;
    }    
    
    return 0;
}