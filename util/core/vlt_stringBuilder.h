#ifndef BASE_STRINGBUILDER_H
#define BASE_STRINGBUILDER_H

#include "config.h"
#include "vlt_memory.h"
#include "vlt_string.h"

typedef struct SStringBuilder {
    U8 *buf;
    U64 bufCap;
    U64 len;
    FMemAlloc *const fAllocMem;
    FMemDealloc *const fDeallocMem;
} StringBuilder;

StringBuilder *new_stringBuilder(FMemAlloc *const fAllocM, FMemDealloc *const fDeallocM);
void del_stringBuilder(StringBuilder *sb);
String *sb_toString(StringBuilder *sb);
void sb_appendString(StringBuilder *sb, String *s);
void sb_appendChar(StringBuilder *sb, U8 ch);
void sb_appendStrLit(StringBuilder *sb, const char *const strLit);

#endif // BASE_STRINGBUILDER_H
