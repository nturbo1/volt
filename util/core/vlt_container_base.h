#ifndef UTIL_CORE_VLT_CONTAINER_BASE_H
#define UTIL_CORE_VLT_CONTAINER_BASE_H

#include "config.h"

typedef Bool FComparator(const void *const k1, const void *const k2);

// A Hash function type
typedef U64 FHash(const U8 *const key, const U64 keySize);

#endif // UTIL_CORE_VLT_CONTAINER_BASE_H
