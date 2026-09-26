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

    // doesn't count the NULL-terminator
    const U64 newStrLen = (len > 0 && bytes[len - 1] == 0) ? len - 1 : len;
    const U8* newStrBytes = (const U8*) malloc(newStrLen + 1);
    ASSERT(newStrBytes != NULL, "Failed to allocate memory for a String object character bytes.");
    ASSERT_DBG(len == 0 || newStrLen <= len,
               "New String length can't be bigger than a given string bytes length.");
    for (U64 i = 0; i < newStrLen; i++)
        *((U8*) &(newStrBytes[i])) = bytes[i];

    *((U8*) &(newStrBytes[newStrLen])) = 0; // append a NULL-terminator
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
    ASSERT( ((source == NULL || source->len == 0) && startIdx == 0) ||
            (source != NULL && source->len > 0),
            "Start index can't be >0 while the source string is NULL or has 0 length." );
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

U8 stringCharAt(String* s, U64 idx)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "String");
    ASSERT(s->len > idx, "String character index %zu is out of bounds.", idx);
    return s->bytes[idx];
}
