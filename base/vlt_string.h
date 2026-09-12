#ifndef VLT_STRING_H
#define VLT_STRING_H

#include "base.h"

#include <stdbool.h>

// Represents an immutable string object
typedef struct
{
    const U8* bytes; // NULL-terminated
    U64 len;
}
String;

// Creates a new String object and returns a pointer to it.
String* new_string(const U8* const bytes, const U64 len);

// Creates a new String object from a string literal.
String* new_stringFromLit(const char* bytes);

// Creates a new `String` obj that contains the bytes from a `source`
// `String` that starts at `startIdx` and ends (not including) at
// `endIdx`.
//
// If `source` is NULL, then returns NULL.
String* stringCopy(const String* const source, const U64 startIdx, const U64 endIdx);

// Deletes a given String object.
// It's the programmer's responsibility to set the string parameter
// pointer to NULL.
void del_string(String* str);

// Compares two strings and returns true if they're equal
bool stringEqual(String* s1, String* s2);

#endif // VLT_STRING_H
