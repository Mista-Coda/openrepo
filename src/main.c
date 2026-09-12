#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NOB_IMPLEMENTATION
#include "../nob.h"

#if defined(_WIN32)
    #define OPEN_CMD "start"
#elif defined(__APPLE__) && defined(__MACH__)
    #define OPEN_CMD "open"
#elif defined(__linux__)
    #define OPEN_CMD "xdg-open"
#else
    #error "Unsupported platform"
#endif

int isHostSep(int c) {
    return (c != ':' && c != '/');
}

int main(int argc, char** argv) {
    nob_shift_args(&argc, &argv);
    Nob_String_View repoPath = {0};

    if (argc > 0) {
        repoPath = nob_sv_from_cstr(argv[0]);
        nob_log(NOB_INFO, "Path provided: %.*s", (int)repoPath.count, repoPath.data);
    } else {
        nob_log(NOB_INFO, "Path not provided");
        repoPath = NOB_SVLIT(".");
    }

    nob_set_current_dir(nob_temp_sv_to_cstr(repoPath));

    // NOTE:
    // While id love to use Nob_Cmd here, from what I know
    // there is no way to read the output of the command without
    // re-directing into a file.
    nob_log(NOB_INFO, "CMD: git remote get-url origin");
    FILE* f = popen("git remote get-url origin", "r");
    if (!f) {
        nob_log(NOB_ERROR, "Failed to run git command");
        return 1;
    }
    char buffer[1024];
    char* output = NULL;
    size_t size = 0;

    while (fgets(buffer, sizeof(buffer), f)) {
        size_t len = strlen(buffer);
        char* newOutput = realloc(output, size + len + 1);
        if (!newOutput) {
            free(output);
            pclose(f);
            fprintf(stderr, "Failed to allocate output buffer");
            return 1;
        }

        output = newOutput;
        memcpy(output + size, buffer, len);
        size += len;
        output[size] = '\0';
    }
    pclose(f);

    Nob_String_View remoteUrl = nob_sv_from_cstr(output);

    size_t stripLen = 0;
    if (nob_sv_starts_with(remoteUrl, NOB_SVLIT("git@"))) {
        printf("[INFO] Uses SSH\n");
        stripLen = 4;
    } else if (nob_sv_starts_with(remoteUrl, NOB_SVLIT("https://"))) {
        printf("[INFO] Uses https\n");
        stripLen = 8;
    } else {
        fprintf(stderr, "[ERROR] Unsupported format.\n");
        return 1;
    }

    nob_sv_chop_left(&remoteUrl, stripLen);
    nob_sv_chop_right(&remoteUrl, 1);

    Nob_String_View path = remoteUrl;
    Nob_String_View host = {0};
    host = nob_sv_chop_while(&path, isHostSep);
    nob_sv_chop_left(&path, 1);

    if (nob_sv_ends_with_cstr(path, ".git")) {
        nob_log(NOB_INFO, "Stripping .git from path");
        nob_sv_chop_right(&path, 4);
    }

    nob_log(NOB_INFO, 
        "Got host and path:\nHost: %.*s\nPath: %.*s",
        (int)host.count, host.data,
        (int)path.count, path.data
    );

    Nob_String_Builder urlSb = {0};
    nob_sb_appendf(&urlSb, "http://%.*s/%.*s",
        (int)host.count, host.data,
        (int)path.count, path.data
    );
    Nob_String_View urlSv = nob_sb_to_sv(urlSb);

    Nob_Cmd open = {0};
    nob_cmd_append(&open, OPEN_CMD, nob_temp_sv_to_cstr(urlSv));

    if (!nob_cmd_run(&open)) return 1;
    return 0;
}