#ifndef UTIL_CORE_VLT_CONTAINER_BASE_H
#define UTIL_CORE_VLT_CONTAINER_BASE_H

#include "config.h"

typedef Bool FComparator(const U8 *const k1, const U8 *const k2, const U64 size);

// A Hash function type
typedef U64 FHash(const U8 *const key, const U64 keySize);

#endif // UTIL_CORE_VLT_CONTAINER_BASE_H
