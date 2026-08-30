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
    const int testInputsSize = 9;
    const StringTestInput inputs[9] = {
        {
            // Ordinary string.
            .strBytes = (U8*) "Hello World!",
            .strBytesLen = 13,
        },
        {
            // Empty string.
            .strBytes = (U8*) "",
            .strBytesLen = 0,
        },
        {
            // Single-character string.
            .strBytes = (U8*) "A",
            .strBytesLen = 1,
        },

        {
            // Single whitespace character.
            .strBytes = (U8*) " ",
            .strBytesLen = 1,
        },
        {
            // Whitespace-only string with different whitespace characters.
            .strBytes = (U8*) " \t\n\r",
            .strBytesLen = 4,
        },
        {
            // Punctuation and special characters.
            .strBytes = (U8*) "!@#$%^&*()_+-=[]{}|;:',.<>/?`~",
            .strBytesLen = 30,
        },
        {
            // Embedded NULL byte.
            // Important: the explicit length means the String should
            // contain all 7 bytes, not stop at the 0x00.
            .strBytes = (U8[]) {
                0x48, 0x65, 0x6C, 0x6C, 0x6F,
                0x00,
                0x57, 0x6F, 0x72, 0x6C, 0x64
            },
            .strBytesLen = 11,
        },
        {
            // Non-ASCII UTF-8 bytes.
            // "Café" = C a f 0xC3 0xA9
            .strBytes = (U8[]) {
                0x43, 0x61, 0x66, 0xC3, 0xA9
            },
            .strBytesLen = 5,
        },
        {
            // Longer string with repeated/patterned data.
            .strBytes = (U8*)
                "The quick brown fox jumps over the lazy dog. "
                "THE QUICK BROWN FOX 0123456789",
            .strBytesLen = 75,
        },
    };

    for (int i = 0; i < testInputsSize; i++)
    {
        // GIVEN
        const U8* const bytes = inputs[i].strBytes;
        const U64 len = inputs[i].strBytesLen;

        // WHEN
        String* s = new_string(bytes, len);

        // THEN
        VCTEST_ASSERT_TRUE(s != NULL);
        VCTEST_ASSERT_TRUE(s->bytes != NULL);
        VCTEST_ASSERT_TRUE(s->len == len);
        VCTEST_ASSERT_TRUE( bytesEqual(s->bytes, bytes, len) ); // check the string bytes are copied

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
        const U64 len = (U64) strlen(lit);;

        // WHEN
        String* s = new_stringFromLit(lit);

        // THEN
        VCTEST_ASSERT_TRUE(s != NULL);
        VCTEST_ASSERT_TRUE(s->bytes != NULL);
        VCTEST_ASSERT_TRUE(s->len == len);
        VCTEST_ASSERT_TRUE( bytesEqual(s->bytes, (const U8* const) lit, len) );
        VCTEST_ASSERT_TRUE( s->bytes != (U8*) lit ); // check the string bytes are copied

        // CLEAN-UP
        del_string(s);
    }
}
