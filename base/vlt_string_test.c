#include "vlt_string.h"
#include "vctest.h"
#include "base.h"

#include <string.h>

typedef struct StringTestInput
{
    const U8* const strBytes;
    const U64 strBytesLen;
}
StringTestInput;

TEST(whenNewString_thenNonNullPointerToStringMustBeReturned,
     "When new_string, then a non-NULL pointer to a String"
     " obj with properly initialized fields MUST be returned")
{
    const int testInputsNullTerminatedSize = 9;
    const StringTestInput inputsNullTerminated[9] = {
        // .strBytesLen values in the inputs counts the NULL-terminator as well
        {
            // Empty string.
            .strBytes = (U8*) "",
            .strBytesLen = 1,
        },
        {
            // Ordinary string.
            .strBytes = (U8*) "Hello World!",
            .strBytesLen = 13,
        },
        {
            // Single-character string.
            .strBytes = (U8*) "A",
            .strBytesLen = 2,
        },

        {
            // Single whitespace character.
            .strBytes = (U8*) " ",
            .strBytesLen = 2,
        },
        {
            // Whitespace-only string with different whitespace characters.
            .strBytes = (U8*) " \t\n\r",
            .strBytesLen = 5,
        },
        {
            // Punctuation and special characters.
            .strBytes = (U8*) "!@#$%^&*()_+-=[]{}|;:',.<>/?`~",
            .strBytesLen = 31,
        },
        {
            // Embedded NULL byte.
            // Important: the explicit length means the String should
            // contain all 12 bytes, not stop at the 0x00.
            .strBytes = (U8[]) {
                0x48, 0x65, 0x6C, 0x6C, 0x6F,
                0x00,
                0x57, 0x6F, 0x72, 0x6C, 0x64,
                0x00
            },
            .strBytesLen = 12,
        },
        {
            // Non-ASCII UTF-8 bytes.
            // "Café" = C a f 0xC3 0xA9
            .strBytes = (U8[]) {
                0x43, 0x61, 0x66, 0xC3, 0xA9, 0x00
            },
            .strBytesLen = 6,
        },
        {
            // Longer string with repeated/patterned data.
            .strBytes = (U8*)
                "The quick brown fox jumps over the lazy dog. "
                "THE QUICK BROWN FOX 0123456789",
            .strBytesLen = 76,
        },
    };

    for (int i = 0; i < testInputsNullTerminatedSize; i++)
    {
        // GIVEN
        const U8* const bytes = inputsNullTerminated[i].strBytes;
        const U64 len = inputsNullTerminated[i].strBytesLen;

        // WHEN
        String* s = new_string(bytes, len);

        // THEN
        VCTEST_ASSERT_TRUE(s != NULL);
        VCTEST_ASSERT_TRUE(s->bytes != NULL);
        VCTEST_ASSERT_TRUE(s->len == len);
        VCTEST_ASSERT_TRUE( bytesEqual(s->bytes, bytes, len) );
        VCTEST_ASSERT_TRUE(s->bytes != bytes); // check the string bytes are copied

        // CLEAN-UP
        del_string(s);
    }

    // Test that a new String obj should add a NULL-terminator
    // when a given string bytes are not NULL-terminated.
    for (int i = 0; i < testInputsNullTerminatedSize; i++)
    {
        // GIVEN
        const U8* const bytes = inputsNullTerminated[i].strBytes;
        const U64 len = inputsNullTerminated[i].strBytesLen - 1; // excluding the NULL-terminator,
                                                                    // simulating the cases where
                                                                    // `strlen` is used to get the
                                                                    // length

        // WHEN
        String* s = new_string(bytes, len);

        // THEN
        VCTEST_ASSERT_TRUE(s != NULL);
        VCTEST_ASSERT_TRUE(s->bytes != NULL);
        VCTEST_ASSERT_TRUE(s->len == len + 1);
        VCTEST_ASSERT_TRUE( bytesEqual(s->bytes, bytes, len) );
        VCTEST_ASSERT_TRUE(s->bytes != bytes); // check the string bytes are copied

        // CLEAN-UP
        del_string(s);
    }
}

TEST(whenNewStringFromLit_thenNonNullPointerToStringMustBeReturned,
     "When new_stringFromLit, then a non-NULL pointer to a String"
     " obj with properly initialized fields MUST be returned")
{
    const int testInputsSize = 8;
    const char* inputs[8] = {
            "Hello World!",
            "",
            "A",
            " ",
            " \t\n\r",
            "!@#$%^&*()_+-=[]{}|;:',.<>/?`~",
            "Café",
            "The quick brown fox jumps over the lazy dog. THE QUICK BROWN FOX 0123456789",
    };

    for (int i = 0; i < testInputsSize; i++)
    {
        // GIVEN
        const char* lit = inputs[i];
        const U64 len = (U64) strlen(lit); // this doesn't count the NULL-terminator

        // WHEN
        String* s = new_stringFromLit(lit);

        // THEN
        VCTEST_ASSERT_TRUE(s != NULL);
        VCTEST_ASSERT_TRUE(s->bytes != NULL);
        VCTEST_ASSERT_TRUE(s->len == len + 1);
        VCTEST_ASSERT_TRUE( bytesEqual(s->bytes, (const U8* const) lit, len) );
        VCTEST_ASSERT_TRUE( s->bytes != (U8*) lit ); // check the string bytes are copied

        // CLEAN-UP
        del_string(s);
    }
}
