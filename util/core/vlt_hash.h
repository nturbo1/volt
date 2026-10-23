#ifndef UTIL_VLT_HASH_H
#define UTIL_VLT_HASH_H

#include "config.h"

// FNV-1a hash algorithm
U64 hash_FNV_1a(const U8 *const bytes, const U64 size);

#endif // UTIL_VLT_HASH_H
