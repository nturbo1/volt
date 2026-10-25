#include "vlt_stringBuilder.h"
#include "base.h"
#include "vlt_assert.h"
#include "vlt_string.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_STRINGBUILDER_BUF_CAP 16

#define SB_MEM_DEALLOCATOR_HAS_NOT_BEEN_INITIALIZED_MSG                                            \
    "StringBuilder memory deallocator has been not initialized!"
#define SB_MEM_ALLOCATOR_HAS_NOT_BEEN_INITIALIZED_MSG                                              \
    "StringBuilder memory allocator has been not initialized!"

static void expandCap(StringBuilder *sb);

StringBuilder *new_stringBuilder(FMemAlloc *const fAllocM, FMemDealloc *const fDeallocM)
{
    FMemAlloc *const fAllocMem = fAllocM ? fAllocM : malloc;
    FMemDealloc *const fDeallocMem = fDeallocM ? fDeallocM : free;

    StringBuilder *sb = (StringBuilder *)fAllocMem(sizeof(StringBuilder));
    ASSERT(sb != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "StringBuilder");
    sb->buf = NULL;
    sb->buf = (U8 *)fAllocMem(DEFAULT_STRINGBUILDER_BUF_CAP);
    ASSERT(sb->buf != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "StringBuilder buffer");
    sb->bufCap = DEFAULT_STRINGBUILDER_BUF_CAP;
    sb->len = 0;
    *(FMemAlloc **)&(sb->fAllocMem) = fAllocMem;
    *(FMemDealloc **)&(sb->fDeallocMem) = fDeallocMem;

    return sb;
}

void del_stringBuilder(StringBuilder *sb)
{
    if (sb) {
        ASSERT(sb->fDeallocMem, SB_MEM_DEALLOCATOR_HAS_NOT_BEEN_INITIALIZED_MSG);
        FMemDealloc *const fDeallocMem = sb->fDeallocMem;
        fDeallocMem(sb->buf);
        fDeallocMem(sb);
    }
}

String *sb_toString(StringBuilder *sb)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "StringBuilder");
    ASSERT(sb->fAllocMem, SB_MEM_ALLOCATOR_HAS_NOT_BEEN_INITIALIZED_MSG);
    ASSERT(sb->fDeallocMem, SB_MEM_DEALLOCATOR_HAS_NOT_BEEN_INITIALIZED_MSG);
    return new_string(sb->buf, sb->len, sb->fAllocMem, sb->fDeallocMem);
}

void sb_appendString(StringBuilder *sb, String *s)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "StringBuilder");
    if (s != NULL)
        for (U64 i = 0; i < s->len; i++) // There is a better way of doing this?
            sb_appendChar(sb, s->bytes[i]);
}

void sb_appendStrLit(StringBuilder *sb, const char *const strLit)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "StringBuilder");
    if (strLit != NULL)
        // There is a better way of doing this?
        for (U64 i = 0; i < strlen(strLit); i++)
            sb_appendChar(sb, strLit[i]);
}

void sb_appendChar(StringBuilder *sb, U8 ch)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "StringBuilder");
    if (sb->bufCap == sb->len)
        expandCap(sb);
    sb->buf[sb->len] = ch;
    sb->len++;
}

static void expandCap(StringBuilder *sb)
{
    ASSERT(sb != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "StringBuilder");
    U64 newBufCap = sb->bufCap * 2;
    ASSERT(sb->fAllocMem, SB_MEM_ALLOCATOR_HAS_NOT_BEEN_INITIALIZED_MSG);
    U8 *newBuf = (U8 *)sb->fAllocMem(newBufCap);
    ASSERT(newBuf != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "StringBuilder buffer");
    for (U64 i = 0; i < sb->len; i++)
        newBuf[i] = sb->buf[i];

    ASSERT(sb->fDeallocMem, SB_MEM_DEALLOCATOR_HAS_NOT_BEEN_INITIALIZED_MSG);
    sb->fDeallocMem(sb->buf);
    sb->buf = newBuf;
    sb->bufCap = newBufCap;
}
