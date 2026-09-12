#include <string.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

int main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    bool enableDebug = false;
    bool install = false;
    if (argc > 1) {
        const char* firstArg = argv[1];
        if (strcmp("debug", firstArg) == 0) {
            enableDebug = true;
            nob_log(NOB_INFO, "Enabling debug");
        } else if (strcmp("install", firstArg) == 0) {
            install = true;
            nob_log(NOB_INFO, "Installing to /usr/bin");
        }
    }

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-o", "openrepo");
    if (enableDebug) {
        cmd_append(&cmd, "-ggdb");
    }

    cmd_append(&cmd, "src/main.c");
    cmd_append(&cmd, "src/stringView.c");

    if (!cmd_run(&cmd)) return 1;

    if (install) {
        nob_log(NOB_ERROR, "Not implemetned");
        return 0;
    }
}