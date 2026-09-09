#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stringView.h"

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

    if (svStartsWith(&sv, "git@")) {
        svStrip(&sv, 4, SV_STRIP_FRONT);
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
    }

    if (repo.host == NULL || repo.path == NULL) {
        fprintf(stderr, "[ERROR] Unsupported format.");
    }

    StringView finalCommand = {0};
    svSetStr(&finalCommand, "xdg-open ");
    svAppend(&finalCommand, "http://");
    svAppend(&finalCommand, repo.host);
    svAppend(&finalCommand, "/");
    svAppend(&finalCommand, repo.path);

    printf("[INFO] %s", finalCommand.characters);
    system(finalCommand.characters);
    return 1;
}