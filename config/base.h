#ifndef BASE_H
#define BASE_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef bool Bool;

#define I8_MAX INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define I8_MIN INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN

#define U8_MIN UINT8_MIN
#define U16_MIN UINT16_MIN
#define U32_MIN UINT32_MIN
#define U64_MIN UINT64_MIN

#define NULL_POINTER_ERROR_MSG_FORMAT "NULL %s pointer was passed!"
#define FAILED_TO_ALLOC_MEM_FOR_FORMAT "Failed to allocate memory for %s."
#define S_ENUM_VALUE_D_IS_OUT_OF_RANGE_FORMAT "%s enum value %d is out of range!"

#endif // BASE_H
