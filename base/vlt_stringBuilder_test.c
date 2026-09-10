#include "vlt_stringBuilder.h"
#include "vctest.h"

TEST(whenNewStringBuilder_thenReturnPointerToNewStringBuilderObjWithCorrectProperties,
     "When new_stringBuilder, then a non-NULL pointer to a new SStringBuilder object"
     " with correctly initialized fields/properties must be returned")
{
    // GIVEN
    // WHEN
    SStringBuilder* sb = new_stringBuilder();
    // THEN
    VCTEST_ASSERT_TRUE(sb != NULL);
    VCTEST_ASSERT_TRUE(sb->buf != NULL);
    VCTEST_ASSERT_TRUE(sb->len == 0);

    // CLEAN-UP
    del_stringBuilder(sb);
}

TEST(whenAppendChar_thenAppendCharToGivenStringBuilderAndUpdateProperly,
     "When sb_appendChar, then append a given char to a given SStringBuilder obj"
     " buffer and update the SStringBuilder object properly")
{
    // GIVEN
    SStringBuilder* sb = new_stringBuilder();
    VCTEST_ASSERT_TRUE(sb != NULL);
    VCTEST_ASSERT_TRUE(sb->buf != NULL);
    U64 inputsSize = 30;
    U8 inputChars[] = {
        0xA7, 0x3C, 0x91, 0xE2, 0x5B, 0x08, 0xD4, 0x6F,
        0xB1, 0x27, 0xC9, 0x54, 0x0E, 0x83, 0xFA, 0x36,
        0x7D, 0xA0, 0x19, 0xCE, 0x42, 0xB8, 0x65, 0xF3,
        0x2A, 0x97, 0xDC, 0x10, 0x4E, 0x7B
    };

    U64 sbLen = sb->len;
    for (U64 i = 0; i < inputsSize; i++)
    {
        // WHEN
        sb_appendChar(sb, inputChars[i]);

        // THEN
        VCTEST_ASSERT_TRUE(sb->buf[sbLen] == inputChars[i]);
        VCTEST_ASSERT_TRUE(sb->len == (++sbLen));
    }

    // CLEAN-UP
    del_stringBuilder(sb);
}
