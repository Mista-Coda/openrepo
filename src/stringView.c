#include "stringView.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void svSetStr(StringView* sv, const char* cStr) {
    size_t stringLength = strlen(cStr);
    size_t stringLengthBytes = stringLength * sizeof(char);
    sv->length = stringLength;
    sv->characters = malloc(stringLengthBytes + 1);
    memcpy(sv->characters, cStr, stringLengthBytes);
    sv->characters[stringLengthBytes] = '\0';
}

void svSetStrAndFree(StringView* sv, char* cStr) {
    svSetStr(sv, cStr);
    free(cStr);
}

void svAppend(StringView* sv, const char* cStr) {
    size_t cStrLength = strlen(cStr);
    size_t newLength = sv->length + cStrLength;

    char* buffer = malloc(newLength + 1);

    memcpy(buffer, sv->characters, sv->length);
    memcpy(buffer + sv->length, cStr, cStrLength);

    buffer[newLength] = '\0';

    free(sv->characters);

    sv->characters = buffer;
    sv->length = newLength;
}

void svStrip(StringView* sv, size_t count, StringViewStripFrom strip) {
    sv->length -= count;
    if (strip == SV_STRIP_BACK) {
        sv->characters[sv->length] = '\0';
    } else if (strip == SV_STRIP_FRONT) {
        sv->characters = sv->characters + count;
    }
}

bool svStartsWith(StringView* sv, const char* with) {
    size_t withLength = strlen(with);
    if (sv->length < withLength) return false;
    
    for (size_t i = 0; i < withLength; ++i) {
        char svLetter = sv->characters[i];
        char withLetter = with[i];
        if (svLetter != withLetter) return false;
    }

    return true;
}

bool svMatchesCStr(StringView* sv, const char* cStr) {
    size_t cstrLength = strlen(cStr);
    if (sv->length != cstrLength) return false;
    for (size_t i = 0; i < cstrLength; ++i) {
        char svChar = sv->characters[i];
        char cStrChar = cStr[i];
        if (svChar != cStrChar) return false;
    }
    return true;
}

bool svSubStr(const StringView* sv, StringView* substr, size_t i, size_t j) {
    if (sv->length < i || sv->length < j || i > j) {
        fprintf(stderr, "[ERROR] Attempt to substring out of bounds");
        return false;
    }

    size_t substrSize = j - i;
    char* buffer = malloc(substrSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "[ERROR] Failed to allocate buffer for sub string");
        return false;
    }

    memcpy(buffer, sv->characters + i, substrSize * sizeof(char));
    buffer[substrSize] = '\0';

    svSetStrAndFree(substr, buffer);
    return true;
}

void svFree(StringView* sv) {
    if (sv->characters != NULL) {
        free(sv->characters);
    } else {
        printf("[WARNING] String view cleared with no allocated string");
    }
}