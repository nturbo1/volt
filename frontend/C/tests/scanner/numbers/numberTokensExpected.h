#ifndef FRONTEND_C_TESTS_SCANNER_NUMBERS_NUMBERTOKENSEXPECTED_H
#define FRONTEND_C_TESTS_SCANNER_NUMBERS_NUMBERTOKENSEXPECTED_H

// decimalSuffixes.c1
#define DEC_SUFF_TOKENS_EXPECTED_SIZE 
const STokenNum DecSuffTokensExpected[] = {
    { .col = 1, .ln = 1, .type = ETOKEN_INT_LIT .val = 0u },
    { .col = 2, .ln = 2, .type = ETOKEN_INT_LIT .val = 1u },
    { .col = 3, .ln = 3, .type = ETOKEN_INT_LIT .val = 42u },
    { .col = 4, .ln = 4, .type = ETOKEN_INT_LIT .val = 123456u },
    { .col = 5, .ln = 5, .type = ETOKEN_INT_LIT .val = 0U },
    { .col = 6, .ln = 6, .type = ETOKEN_INT_LIT .val = 1U },
    { .col = 7, .ln = 7, .type = ETOKEN_INT_LIT .val = 42U },
    { .col = 8, .ln = 8, .type = ETOKEN_INT_LIT .val = 123456U },
    { .col = 9, .ln = 9, .type = ETOKEN_INT_LIT .val = 0l },
    { .col = 10, .ln = 10, .type = ETOKEN_INT_LIT .val = 1l },
    { .col = 11, .ln = 11, .type = ETOKEN_INT_LIT .val = 42l },
    { .col = 12, .ln = 12, .type = ETOKEN_INT_LIT .val = 123456l },
    { .col = 13, .ln = 13, .type = ETOKEN_INT_LIT .val = 0L },
    { .col = 14, .ln = 14, .type = ETOKEN_INT_LIT .val = 1L },
    { .col = 15, .ln = 15, .type = ETOKEN_INT_LIT .val = 42L },
    { .col = 16, .ln = 16, .type = ETOKEN_INT_LIT .val = 123456L },
    { .col = 17, .ln = 17, .type = ETOKEN_INT_LIT .val = 0ul },
    { .col = 18, .ln = 18, .type = ETOKEN_INT_LIT .val = 1ul },
    { .col = 19, .ln = 19, .type = ETOKEN_INT_LIT .val = 42ul },
    { .col = 20, .ln = 20, .type = ETOKEN_INT_LIT .val = 123456ul },
    { .col = 22, .ln = 22, .type = ETOKEN_INT_LIT .val = 0UL },
    { .col = 23, .ln = 23, .type = ETOKEN_INT_LIT .val = 1UL },
    { .col = 24, .ln = 24, .type = ETOKEN_INT_LIT .val = 42UL },
    { .col = 25, .ln = 25, .type = ETOKEN_INT_LIT .val = 123456UL },
    { .col = 26, .ln = 26, .type = ETOKEN_INT_LIT .val = 0lu },
    { .col = 27, .ln = 27, .type = ETOKEN_INT_LIT .val = 1lu },
    { .col = 28, .ln = 28, .type = ETOKEN_INT_LIT .val = 42lu },
    { .col = 29, .ln = 29, .type = ETOKEN_INT_LIT .val = 123456lu },
    { .col = 30, .ln = 30, .type = ETOKEN_INT_LIT .val = 0LU },
    { .col = 31, .ln = 31, .type = ETOKEN_INT_LIT .val = 1LU },
    { .col = 32, .ln = 32, .type = ETOKEN_INT_LIT .val = 42LU },
    { .col = 33, .ln = 33, .type = ETOKEN_INT_LIT .val = 123456LU },
    { .col = 34, .ln = 34, .type = ETOKEN_INT_LIT .val = 0ll },
    { .col = 35, .ln = 35, .type = ETOKEN_INT_LIT .val = 1ll },
    { .col = 36, .ln = 36, .type = ETOKEN_INT_LIT .val = 42ll },
    { .col = 37, .ln = 37, .type = ETOKEN_INT_LIT .val = 123456ll },
    { .col = 38, .ln = 38, .type = ETOKEN_INT_LIT .val = 0LL },
    { .col = 39, .ln = 39, .type = ETOKEN_INT_LIT .val = 1LL },
    { .col = 40, .ln = 40, .type = ETOKEN_INT_LIT .val = 42LL },
    { .col = 41, .ln = 41, .type = ETOKEN_INT_LIT .val = 123456LL },
    { .col = 42, .ln = 42, .type = ETOKEN_INT_LIT .val = 0ull },
    { .col = 43, .ln = 43, .type = ETOKEN_INT_LIT .val = 1ull },
    { .col = 44, .ln = 44, .type = ETOKEN_INT_LIT .val = 42ull },
    { .col = 45, .ln = 45, .type = ETOKEN_INT_LIT .val = 123456ull },
    { .col = 46, .ln = 46, .type = ETOKEN_INT_LIT .val = 0ULL },
    { .col = 47, .ln = 47, .type = ETOKEN_INT_LIT .val = 1ULL },
    { .col = 48, .ln = 48, .type = ETOKEN_INT_LIT .val = 42ULL },
    { .col = 49, .ln = 49, .type = ETOKEN_INT_LIT .val = 123456ULL },
    { .col = 50, .ln = 50, .type = ETOKEN_INT_LIT .val = 0llu },
    { .col = 51, .ln = 51, .type = ETOKEN_INT_LIT .val = 1llu },
    { .col = 52, .ln = 52, .type = ETOKEN_INT_LIT .val = 42llu },
    { .col = 53, .ln = 53, .type = ETOKEN_INT_LIT .val = 123456llu },
    { .col = 54, .ln = 54, .type = ETOKEN_INT_LIT .val = 0LLU },
    { .col = 55, .ln = 55, .type = ETOKEN_INT_LIT .val = 1LLU },
    { .col = 56, .ln = 56, .type = ETOKEN_INT_LIT .val = 42LLU },
    { .col = 57, .ln = 57, .type = ETOKEN_INT_LIT .val = 123456LLU },
};
 
// decimals.c1
// floats.c1
// floatsSuffixes.c1
// hexFloats.c1
// hexaSuffixes.c1
// hexas.c1
// largeAndSmallFloats.c1
// longDigitFloats.c1
// multipleNumbersOnOneLine.c1
// numberTokenBoundaries.c1
// octalSuffixes.c1
// octals.c1

#endif // FRONTEND_C_TESTS_SCANNER_NUMBERS_NUMBERTOKENSEXPECTED_H
