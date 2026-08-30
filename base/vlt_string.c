#include "vlt_string.h"
#include "vlt_assert.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

String* new_string(const U8* const bytes, const U64 len)
{
    String* new_str = (String*) malloc(sizeof(String));
    ASSERT(new_str != NULL, "Failed to allocate memory for a String object.");
    const U8* str_bytes = (const U8*) malloc(len);
    ASSERT(str_bytes != NULL, "Failed to allocate memory for a String object character bytes.");
    for (U64 i = 0; i < len; i++)
        *((U8*) &(str_bytes[i])) = bytes[i];
    new_str->bytes = str_bytes;
    str_bytes = NULL;
    new_str->len = len;

    return new_str;
}

String* new_stringFromLit(const char* bytes)
{
    return new_string((const U8* const) bytes, strlen(bytes));
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
