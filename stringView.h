#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char* characters;
    size_t length;
} StringView;

typedef enum {
    SV_STRIP_BACK,
    SV_STRIP_FRONT,
} StringViewStripFrom;

void svSetStr(StringView* sv, const char* cStr);
void svSetStrAndFree(StringView* sv, char* cStr);
void svAppend(StringView* sv, const char* cStr);
void svStrip(StringView* sv, size_t count, StringViewStripFrom strip);
bool svStartsWith(StringView* sv, const char* with);
bool svMatchesCStr(StringView* sv, const char* cStr);
bool svSubStr(const StringView* sv, StringView* substr, size_t i, size_t j);
void svFree(StringView* sv);