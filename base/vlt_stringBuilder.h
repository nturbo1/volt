#ifndef BASE_STRINGBUILDER_H
#define BASE_STRINGBUILDER_H

#include "base.h"
#include "vlt_string.h"

typedef struct StringBuilder
{
    U8* buf;
    U64 bufCap;
    U64 len;
}
SStringBuilder;

SStringBuilder* new_stringBuilder();
void del_stringBuilder(SStringBuilder* sb);
String* sb_toString(SStringBuilder* sb);
void sb_appendString(SStringBuilder* sb, String* s);
void sb_appendChar(SStringBuilder* sb, U8 ch);
void sb_appendStrLit(SStringBuilder* sb, const char* const strLit);

#endif // BASE_STRINGBUILDER_H
