#ifndef VLT_STRING_H
#define VLT_STRING_H

#include "base.h"

// Represents an immutable string object
typedef struct
{
    const U8* bytes;
    U64 len;
}
String;

// Creates a new String object and returns a pointer to it.
String* vlt_new_string(const U8* const bytes, const U64 len);

// Creates a new String object from a string literal.
String* vlt_new_string_from_lit(const char* bytes);

// Deletes a given String object.
// It's the programmer's responsibility to set the string parameter
// pointer to NULL.
Bool vlt_del_string(String* str);

#endif // VLT_STRING_H
