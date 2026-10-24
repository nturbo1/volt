#ifndef UTIL_CORE_MEMORY_H
#define UTIL_CORE_MEMORY_H

#include "config.h"

typedef void *FMemAlloc(U64 size);
typedef void FMemDealloc(void *ptr);

#endif // UTIL_CORE_MEMORY_H
