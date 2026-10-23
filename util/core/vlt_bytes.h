#ifndef UTIL_VLT_BYTES_H
#define UTIL_VLT_BYTES_H

#include "config.h"

Bool bytesEqual(const U8 *const bytes1, const U8 *const bytes2, const U64 bytesSize);
U8 *copyBytesToNew(const U8 *const bytes, const U64 size);
void copyBytesFromTo(const U8 *const source, const U64 size, U8 *const dest);

#endif // UTIL_VLT_BYTES_H
