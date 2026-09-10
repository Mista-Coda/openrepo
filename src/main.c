#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stringView.h"

#if defined(_WIN32)
    #define OPEN_CMD "start "
#elif defined(__APPLE__) && defined(__MACH__)
    #define OPEN_CMD "open "
#elif defined(__linux__)
    #define OPEN_CMD "xdg-open "
#else
    #error "Unsupported platform
#endif


typedef struct {
    char* host;
    char* path;
} RepoHost;

int main() {
    printf("[INFO] git remote get-url origin\n");
    FILE* f = popen("git remote get-url origin", "r");
    if (!f) {
        fprintf(stderr, "Failed to run git command");
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

    StringView sv = {0};
    svSetStrAndFree(&sv, output);

    RepoHost repo = {0};
    StringView hostStr = {0};
    StringView pathStr = {0};

    size_t stripLen = 0;
    if (svStartsWith(&sv, "git@")) {
        printf("[INFO] Uses SSH");
        stripLen = 4;
    } else if (svStartsWith(&sv, "https://")) {
        printf("[INFO] Uses https");
        stripLen = 8;
    } else {
        fprintf(stderr, "[ERROR] Unsupported format.\n");
        return 1;
    }

    svStrip(&sv, stripLen, SV_STRIP_FRONT);

    // This is hardcoded for a github.com length hostname.
    bool didSub = svSubStr(&sv, &hostStr, 0, 10);
    if (!didSub) {
        fprintf(stderr, "[ERROR] Failed to substr the hostname");
        return 1;
    }

    didSub = svSubStr(&sv, &pathStr, 11, sv.length);
    if (!didSub) {
        fprintf(stderr, "[ERROR] Failed to substr the path");
        return 1;
    }

    repo.host = hostStr.characters;
    repo.path = pathStr.characters;

    if (repo.host == NULL || repo.path == NULL) {
        fprintf(stderr, "[ERROR] Failed to extract host or path");
        return 1;
    }

    StringView finalCommand = {0};
    svSetStr(&finalCommand, OPEN_CMD);
    svAppend(&finalCommand, "http://");
    svAppend(&finalCommand, repo.host);
    svAppend(&finalCommand, "/");
    svAppend(&finalCommand, repo.path);

    printf("[INFO] %s", finalCommand.characters);
    system(finalCommand.characters);
    return 1;
}