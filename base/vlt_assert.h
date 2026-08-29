#ifndef BASE_VLT_ASSERT_H
#define BASE_VLT_ASSERT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define ASSERT_IMPL(cond, ...)                                      \
    do {                                                            \
        if (!(cond)) {                                              \
            fprintf(stderr,                                         \
                    "[%s:%d]: Assertion (%s) failed.\n\t",          \
                    __FILE__, __LINE__, #cond);                     \
            fprintf(stderr, __VA_ARGS__);                           \
            fputc('\n', stderr);                                    \
            abort();                                                \
        }                                                           \
    } while (0)

#define ASSERT(cond, ...) ASSERT_IMPL(cond, __VA_ARGS__)

#ifdef DEBUG
# define ASSERT_DBG(cond, ...) ASSERT_IMPL(cond, __VA_ARGS__)
#else
# define ASSERT_DBG(...) do {} while (0)
#endif

#endif // BASE_VLT_ASSERT_H
