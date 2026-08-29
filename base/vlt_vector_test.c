#include "vctest.h"
#include "base_inc.h"
#include "vlt_vector.h"

TEST(whenNewVec_thenNonNullVecWithValidProps,
     "When create a new Vec obj, then non-NULL obj must be returned and the obj"
     " must have valid properties/fields")
{
    // GIVEN
    const U64 vLen = 0;
    const U64 vCap = 4;
    const U64 vElemSize = 2;

    // WHEN
    Vec* v = new_vec(vLen, vCap, vElemSize);

    // THEN
    VCTEST_ASSERT_TRUE(v != NULL);
    VCTEST_ASSERT_TRUE(v->len == vLen);
    VCTEST_ASSERT_TRUE(v->cap == vCap);
    VCTEST_ASSERT_TRUE(v->elemSize == vElemSize);
    VCTEST_ASSERT_TRUE(v->buf != NULL);

    // CLEAN-UP
    del_vec(v);
    v = NULL;
}

TEST(whenVecPush_thenTheElemIsAppendedToVecAndVecIsUpdatedPropertly,
     "When vec_push an element, then the element must be appended to the Vec"
     " obj buffer and the Vec obj must properly updated")
{
    // GIVEN
    const U64 vLen = 0;
    const U64 vCap = 4;
    const U64 vElemSize = 4;
    const U8 vElem[4] = { 0xdd, 0x3f, 0x34, 0x9a };

    // WHEN
    Vec* v = new_vec(vLen, vCap, vElemSize);
    VCTEST_ASSERT_TRUE(v != NULL);
    vec_push(v, vElemSize, vElem);

    // THEN
    VCTEST_ASSERT_TRUE(v->cap == vCap);
    VCTEST_ASSERT_TRUE(v->elemSize == vElemSize);
    VCTEST_ASSERT_TRUE(v->buf != NULL);
    VCTEST_ASSERT_TRUE(v->len == (vLen + 1));
    for (U64 i = 0; i < vElemSize; i++)
        VCTEST_ASSERT_TRUE(v->buf[(vLen * vElemSize) + i] == vElem[i]);

    // CLEAN-UP
    del_vec(v);
    v = NULL;
}

TEST(whenVecInsert_thenTheElemIsInsertedToVecAtIndex,
     "When vec_insert an element, then the element must be inserted at the"
     "specified index and the Vec obj buffer updated")
{
    // GIVEN
    const U64 vLen = 5;
    const U64 vCap = 8;
    const U64 vElemSize = 4;
    const U8 vElem[4] = { 0xdd, 0x3f, 0x34, 0x9a };
    const U64 vIdx = 2;

    // WHEN
    Vec* v = new_vec(vLen, vCap, vElemSize);
    VCTEST_ASSERT_TRUE(v != NULL);
    vec_insert(v, vIdx, vElem, vElemSize);

    // THEN
    VCTEST_ASSERT_TRUE(v->cap == vCap);
    VCTEST_ASSERT_TRUE(v->elemSize == vElemSize);
    VCTEST_ASSERT_TRUE(v->buf != NULL);
    VCTEST_ASSERT_TRUE(v->len == vLen);
    for (U64 i = 0; i < vElemSize; i++)
        VCTEST_ASSERT_TRUE(v->buf[(vIdx * vElemSize) + i] == vElem[i]);

    // CLEAN-UP
    del_vec(v);
    v = NULL;
}
