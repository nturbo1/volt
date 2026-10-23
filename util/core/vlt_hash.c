#include "vlt_hash.h"
#include "config.h"
#include "vlt_assert.h"

// Parameters for FNV Hash algorithms
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

U64 hash_FNV_1a(const U8 *const bytes, const U64 size)
{
    U64 hashval = FNV_OFFSET;

    for (U64 i = 0; i < size; i++) {
        hashval ^= (U64)(U8)bytes[i];
        hashval *= FNV_PRIME;
    }

    return hashval;
}
