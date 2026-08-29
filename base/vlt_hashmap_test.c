#include "vlt_hashmap.h"
#include "vctest.h"

#include <stdbool.h>

TEST(whenNewHMap_thenNewHMapIsCreatedAndHMapObjIsInitializedProperlyAndPointerToNewHMapIsReturned,
     "When new_hmap, then a new HMap obj is created with properly"
     " initialized fields and a pointer to the HMap obj is returned")
{
    HMap* hm = new_hmap();
    VCTEST_ASSERT_TRUE(hm != NULL);
    VCTEST_ASSERT_TRUE(hm->size == 0);

    del_hmap(hm);
    hm = NULL;
}
