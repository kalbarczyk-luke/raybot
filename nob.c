#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return -1;

    nob_cmd_append(&cmd, "cc", "-o", BUILD_FOLDER"main", SRC_FOLDER"main.c",
                    "-lraylib", "-lGL", "-lm", "-lpthread", "-ldl", "-lrt", "-lX11");

    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        nob_log(NOB_ERROR, "Compilation failed.");
        return 1;
    }

    nob_log(NOB_INFO, "Build successful.");

    nob_cmd_append(&cmd, "./build/main");

    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        nob_log(NOB_ERROR, "Run failed.");
        return 1;
    }

    return 0;
}

// gcc game.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11