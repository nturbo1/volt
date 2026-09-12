#include "vlt_string.h"
#include "vlt_assert.h"
#include "base.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

String* new_string(const U8* const bytes, const U64 len)
{
    String* newStr = (String*) malloc(sizeof(String));
    ASSERT(newStr != NULL, "Failed to allocate memory for a String object.");
    const U64 newStrLen = bytes[len - 1] == 0 ? len : len + 1;
    const U8* newStrBytes = (const U8*) malloc(newStrLen);
    ASSERT(newStrBytes != NULL, "Failed to allocate memory for a String object character bytes.");
    for (U64 i = 0; i < len; i++)
        *((U8*) &(newStrBytes[i])) = bytes[i];
    *((U8*) &(newStrBytes[newStrLen - 1])) = 0;
    newStr->bytes = newStrBytes;
    newStrBytes = NULL;
    newStr->len = newStrLen;

    return newStr;
}

String* new_stringFromLit(const char* bytes)
{
    return new_string((const U8* const) bytes, strlen(bytes));
}

String* stringCopy(const String* const source, const U64 startIdx, const U64 endIdx)
{
    ASSERT((source == NULL || source->len == 0) && startIdx > 0,
           "Start index can't be >0 while the source string is NULL or has 0 length.");
    ASSERT(startIdx < endIdx, "End index can't be less than or equal to start index.");
    ASSERT(endIdx <= source->len, "End index exceeds the string length.");

    if (source == NULL)
        return NULL;

    return new_string(source->bytes + startIdx, endIdx - startIdx);
}

void del_string(String* str)
{
    if (str == NULL)
        return;

    free((U8*) str->bytes);
    str->bytes = NULL;
    str->len = 0;
    free(str);
}

bool stringEqual(String* s1, String* s2)
{
    if (s1 == NULL || s2 == NULL)
        return false;

    return s1->len == s2->len && bytesEqual(s1->bytes, s2->bytes, s1->len);
}
