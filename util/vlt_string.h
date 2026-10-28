#ifndef VLT_STRING_H
#define VLT_STRING_H

#include "config.h"
#include "core/vlt_assert.h"
#include "core/vlt_hash.h"
#include "core/vlt_memory.h"

// Represents an immutable string object
typedef struct {
    const U8 *bytes; // NULL-terminated
    const U64 len; // doesn't count the NULL-terminator
    FMemAlloc *const fAllocMem;
    FMemDealloc *const fDeallocMem;
} String;

// Creates a new String object and returns a pointer to it.
//
// If given bytes are not NULL-terminated, then it appends a
// NULL-terminator to the bytes of a newly created `String` obj.
// However `len` of a newly created `String` obj doesn't count
// the NULL-terminator.
String *new_string(const U8 *const bytes,
                   const U64 len,
                   FMemAlloc *const fAllocMem,
                   FMemDealloc *const fDeallocMem);

// Creates a new String object from a string literal.
String *new_stringFromLit(const char *bytes,
                          FMemAlloc *const fAllocMem,
                          FMemDealloc *const fDeallocMem);

// Creates a new `String` obj that contains the bytes from a `source`
// `String` that starts at `startIdx` and ends (not including) at
// `endIdx`.
//
// If `source` is NULL, then returns NULL.
String *string_copy(const String *const source, const U64 startIdx, const U64 endIdx);

// Deletes a given String object.
// It's the programmer's responsibility to set the string parameter
// pointer to NULL.
void del_string(String *str);

// Compares two strings and returns true if they're equal
Bool stringEqual(String *s1, String *s2);

// Returns a character at a given index in a given String
U8 stringCharAt(String *s, U64 idx);

inline U64 string_len(const String *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "String");
    return s->len;
}

inline Bool string_isEmpty(const String *const s)
{
    return string_len(s) > 0;
}

// Used by containers/collections that store elements or keys of type String.
inline void string_memDealloc(void *s)
{
    String *str = (String *)s;
    if (str)
        del_string(str);
}

inline U64 string_hash(const U8 *const str, const U64 size)
{
    ASSERT_DBG(size == sizeof(String), "String obj bytes size doesn't match the String type size.");
    String *s = (String *)str;
    return hash_FNV_1a(s->bytes, s->len);
}

#endif // VLT_STRING_H
