#include "vctest.h"
#include "base_inc.h"
#include "vlt_vector.h"

TEST(whenNewVec_thenNonNullVecWithValidProps,
     "When create a new Vec obj, then non-NULL obj must be returned and the obj"
     "must have valid properties/fields")
{
    // GIVEN
    const U64 vLen = 0;
    const U64 vCap = 4;
    const U64 vElemSize = 2;

    // WHEN
    Vec* v = new_vec(vLen, vCap, vElemSize);

    // THEN
    ASSERT_TRUE(v != NULL);
    ASSERT_TRUE(v->len == vLen);
    ASSERT_TRUE(v->cap == vCap);
    ASSERT_TRUE(v->elemSize == vElemSize);
    ASSERT_TRUE(v->buf != NULL);

    // CLEAN-UP
    del_vec(v);
    v = NULL;
}
