#ifndef VCTEST_H
#define VCTEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef void (*VCTestFn)(void);

typedef struct {
    const char* name;
    VCTestFn   fn;
    const char* description;
} VCTest;

typedef struct {
    const char* test_name;
    const char* fail_file;
    int         fail_line;
    bool        failed;
    char        fail_msg[256];
} VCTestState;

extern VCTestState _vctest_curr_test_state;

#define TEST(name, description)                                \
    void VCTest_##name(void);                                  \
    static VCTest _nt_test_##name                              \
        __attribute__((section("test_registry"),               \
                    aligned(1),                                \
                    used)) =                                   \
        { #name, VCTest_##name, description };                 \
    void VCTest_##name(void)

#define VCTEST_ASSERT_TRUE(cond) \
    do {                                                        \
        if (!(cond)) {                                          \
            _vctest_curr_test_state.failed = true;              \
            _vctest_curr_test_state.fail_file = __FILE__;       \
            _vctest_curr_test_state.fail_line = __LINE__;       \
            snprintf(_vctest_curr_test_state.fail_msg,          \
                    sizeof(_vctest_curr_test_state.fail_msg),   \
                    "ASSERT_TRUE(%s) failed",                   \
                    #cond);                                     \
            return;                                             \
        }                                                       \
    } while(0)

void vctest_run_all(bool verbose);

#define PRINT_HELP() \
    do { \
        printf("Usage:\n\tvctest\n\tvctest -v\n\tvctest --verbose\n"); \
    } while(0)

#endif // VCTEST_H 
