#include "vlt_stringBuilder.h"
#include "vlt_assert.h"
#include "vlt_string.h"
#include "base.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_STRINGBUILDER_BUF_CAP 16

static void expandCap(SStringBuilder* sb);

SStringBuilder* new_stringBuilder()
{
    SStringBuilder* sb = (SStringBuilder*) malloc(sizeof(SStringBuilder));
    ASSERT(sb != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SStringBuilder");
    sb->buf = NULL;
    sb->buf = (U8*) malloc(DEFAULT_STRINGBUILDER_BUF_CAP);
    ASSERT(sb->buf != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SStringBuilder buffer");
    sb->bufCap = DEFAULT_STRINGBUILDER_BUF_CAP;
    sb->len = 0;

    return sb;
}

void del_stringBuilder(SStringBuilder* sb)
{
    if (sb != NULL)
    {
        free(sb->buf);
        free(sb);
    }
}

String* sb_ToString(SStringBuilder* sb)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SStringBuilder");
    return new_string(sb->buf, sb->len);
}

void sb_appendString(SStringBuilder* sb, String* s)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SStringBuilder");
    if (s != NULL)
        for (U64 i = 0; i < s->len; i++) // There is a better way of doing this?
            sb_appendChar(sb, s->bytes[i]);
}

void sb_appendStrLit(SStringBuilder* sb, const char* const strLit)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SStringBuilder");
    if (strLit != NULL)
        // There is a better way of doing this?
        for (U64 i = 0; i < strlen(strLit); i++)
            sb_appendChar(sb, strLit[i]);
}

void sb_appendChar(SStringBuilder* sb, U8 ch)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SStringBuilder");
    if (sb->bufCap == sb->len)
        expandCap(sb);
    sb->buf[sb->len] = ch;
    sb->len++;
}

static void expandCap(SStringBuilder* sb)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SStringBuilder");
    U64 newBufCap = sb->bufCap * 2;
    U8* newBuf = (U8*) malloc(newBufCap);
    ASSERT(newBuf != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SStringBuilder buffer");
    for (U64 i = 0; i < sb->len; i++)
        newBuf[i] = sb->buf[i];

    free(sb->buf);
    sb->buf = newBuf;
    sb->bufCap = newBufCap;
}
