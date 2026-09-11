#include "vlt_stringBuilder.h"
#include "vctest.h"
#include "vlt_string.h"
#include "base.h"

#include <string.h>

TEST(whenNewStringBuilder_thenReturnPointerToNewStringBuilderObjWithCorrectProperties,
     "When new_stringBuilder, then a non-NULL pointer to a new"
     " SStringBuilder object with correctly initialized"
     " fields/properties must be returned")
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
     "When sb_appendChar, then append a given char to a given"
     " SStringBuilder obj buffer and update the SStringBuilder"
     " object properly")
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

TEST(whenAppendString_thenAppendStringToGivenStringBuilderAndUpdateProperly,
     "When sb_appendString, then append a given String bytes to a given"
     " SStringBuilder obj buffer and update the SStringBuilder object properly")
{
    U64 inputsSize = 5;
    String* inputs[] = {
        new_stringFromLit("Hello"),
        new_stringFromLit("Hello World"),
        new_stringFromLit("908asg908a;kla"),
        new_stringFromLit("0'ldsgqkh-0}kqk--a9env;ks;"),
        new_stringFromLit(""),
    };

    for (U64 i = 0; i < inputsSize; i++)
    {
        // GIVEN
        SStringBuilder* sb = new_stringBuilder();
        VCTEST_ASSERT_TRUE(sb != NULL);
        VCTEST_ASSERT_TRUE(sb->buf != NULL);
        String* str = inputs[i];

        // WHEN
        sb_appendString(sb, str);

        // THEN
        VCTEST_ASSERT_TRUE(sb->len == str->len);
        VCTEST_ASSERT_TRUE(bytesEqual(sb->buf, str->bytes, str->len));

        // CLEAN-UP
        del_stringBuilder(sb);
    }

    // CLEAN-UP
    for (U64 i = 0; i < inputsSize; i++)
        del_string(inputs[i]);
}

TEST(whenAppendStrLit_thenAppendStrLitToGivenStringBuilderAndUpdateProperly,
     "When sb_appendStrLit, then append a given string literal bytes to a"
     " given SStringBuilder obj buffer and update the SStringBuilder object"
     " properly")
{
    U64 inputsSize = 5;
    const char* const inputs[] = {
        "Hello",
        "Hello World",
        "908asg908a;kla",
        "0'ldsgqkh-0}kqk--a9env;ks;",
        "",
    };

    for (U64 i = 0; i < inputsSize; i++)
    {
        // GIVEN
        SStringBuilder* sb = new_stringBuilder();
        VCTEST_ASSERT_TRUE(sb != NULL);
        VCTEST_ASSERT_TRUE(sb->buf != NULL);
        const char* const strLit = inputs[i];
        const U64 strLitLen = strlen(strLit);

        // WHEN
        sb_appendStrLit(sb, strLit);

        // THEN
        VCTEST_ASSERT_TRUE(sb->len == strLitLen);
        VCTEST_ASSERT_TRUE(bytesEqual(sb->buf, (const U8* const) strLit, strLitLen));

        // CLEAN-UP
        del_stringBuilder(sb);
    }
}

TEST(whenToString_thenReturnCorrectString,
     "When sb_ToString, then return a pointer to a String obj that"
     " contains correct bytes")
{
    U64 inputsSize = 5;
    String* inputs[] = {
        new_stringFromLit("Hello"),
        new_stringFromLit("Hello World"),
        new_stringFromLit("908asg908a;kla"),
        new_stringFromLit("0'ldsgqkh-0}kqk--a9env;ks;"),
        new_stringFromLit(""),
    };

    for (U64 i = 0; i < inputsSize; i++)
    {
        // GIVEN
        SStringBuilder* sb = new_stringBuilder();
        VCTEST_ASSERT_TRUE(sb != NULL);
        VCTEST_ASSERT_TRUE(sb->buf != NULL);
        String* str = inputs[i];
        sb_appendString(sb, str);

        // WHEN
        String* sbStr = sb_ToString(sb);

        // THEN
        VCTEST_ASSERT_TRUE(sbStr->len == str->len);
        VCTEST_ASSERT_TRUE(bytesEqual(sbStr->bytes, str->bytes, str->len));

        // CLEAN-UP
        del_stringBuilder(sb);
        del_string(sbStr);
    }

    // CLEAN-UP
    for (U64 i = 0; i < inputsSize; i++)
        del_string(inputs[i]);
}
