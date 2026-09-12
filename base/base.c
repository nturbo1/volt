#include "base.h"
#include "vlt_assert.h"

#include <stdlib.h>

bool bytesEqual(const U8* const bytes1, const U8* const bytes2, const U64 bytesSize)
{
    for (U64 i = 0; i < bytesSize; i++)
        if (bytes1[i] != bytes2[i])
            return false;

    return true;
}

U8* copyBytesToNew(const U8* const bytes, const U64 size)
{
    U8* newBytes = (U8*) malloc(size);
    ASSERT(newBytes != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "copied bytes");
    for (U64 i = 0; i < size; i++)
        newBytes[i] = bytes[i];

    return newBytes;
}

// Parameters for FNV Hash algorithms
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

U64 hash_FNV_1a(const U8* const bytes, const U64 size) {
    U64 hashval = FNV_OFFSET;

    for (U64 i = 0; i < size; i++)
    {
        hashval ^= (U64)(U8) bytes[i];
        hashval *= FNV_PRIME;
    }

    return hashval;
}
