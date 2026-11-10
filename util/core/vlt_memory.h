#ifndef UTIL_CORE_MEMORY_H
#define UTIL_CORE_MEMORY_H

#include "config.h"

typedef void *FMemAlloc(U64 size);
typedef void FMemDealloc(void *ptr);

typedef struct MemAllocator {
    FMemAlloc *const fAlloc;
    FMemDealloc *const fDealloc;
} MemAllocator;

#endif // UTIL_CORE_MEMORY_H
