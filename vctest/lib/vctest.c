#include "vctest.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

extern VCTest __start_test_registry;
extern VCTest __stop_test_registry;

VCTestState _vctest_curr_test_state = {
    .test_name = NULL,
    .fail_file = NULL,
    .fail_line = 0,
    .failed = false // will be set to false by asserts in case they fail
};

#define PASS_COLORED_TEXT "\033[38;5;47mPASS\033[m"
#define FAIL_COLORED_TEXT "\033[38;5;197mFAIL\033[m"

#define PRINT_TEST_STAT(passed_count, failed_count)                                                      \
    do {                                                                                                 \
        printf("\n---------------------------------------------------------------------------------\n"); \
        printf("TOTAL: %zu tests\n", passed_count + failed_count);                                       \
        printf("PASSED: %zu\n", passed_count);                                                           \
        printf("FAILED: %zu\n", failed_count);                                                           \
        printf("---------------------------------------------------------------------------------\n\n"); \
        fflush(stdout);                                                                                  \
    } while(0)

#define PRINT_SINGLE_TEST_RES(t)                                        \
    do {                                                                \
        if (t->failed) {                                                \
            printf("["FAIL_COLORED_TEXT"] %s\n", t->test_name);         \
            printf("        %s\n", t->fail_msg);                        \
            printf("        at %s:%d\n", t->fail_file, t->fail_line);   \
        } else {                                                        \
            printf("["PASS_COLORED_TEXT"] %s\n", t->test_name);         \
        }                                                               \
        fflush(stdout);                                                 \
    } while(0)

void vctest_run_all(bool verbose)
{
    printf("Running tests...\n");

    size_t passed_count = 0;
    size_t failed_count = 0;

    for (VCTest* t = &__start_test_registry; t < &__stop_test_registry; t++)
    {
        _vctest_curr_test_state = (VCTestState) {.test_name = t->name };

        if (t->fn == NULL)
            continue;

        t->fn();

        if (_vctest_curr_test_state.failed) {
            ++failed_count;
        } else {
            ++passed_count;
        }

        if (verbose) {
            PRINT_SINGLE_TEST_RES((&_vctest_curr_test_state));
        }
    }

    PRINT_TEST_STAT(passed_count, failed_count);
}
