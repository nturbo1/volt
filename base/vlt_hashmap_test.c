#include "vlt_hashmap.h"
#include "vctest.h"

#include <stdbool.h>

typedef struct HMapTestInput
{
    const U8* key;
    const U64 keySize;
    U8* val;
    const U64 valSize;
}
HMapTestInput;

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

TEST(whenHMapPut_thenTheValueWithKeyMustBeAddedAndRetrievedByKeyAndHMapObjMustBeUpdatedProperly,
     "When hmap_put, then the given value with the key MUST"
     " be put in the hashmap, retrieved by the key, and the"
     " hashmap object MUST be properly updated")
{
    int testInputsSize = 1;
    HMapTestInput inputs[] = {
        {
            .key = (const U8[]) { 0xA7, 0x3C, 0x91, 0xE2, 0x5B, 0x08, 0xD4,
                     0x6F, 0xB9, 0x21, 0xC6, 0x4A, 0xF0, 0x7D },
            .keySize = 14,
            .val = (U8[]) {
                0x4E, 0xA1, 0x7B, 0xD3, 0x29, 0xF8, 0x65, 0x0C,
                0xB7, 0x42, 0x9D, 0xE6, 0x13, 0x58, 0xCA, 0x34,
                0x8F, 0xD0, 0x76, 0x2B, 0xA9, 0x45, 0xE1, 0xBC,
                0x07, 0x93, 0x5D, 0xF2, 0x68, 0xC4, 0x1A, 0x87,
                0xDB, 0x30, 0xAE, 0x59, 0xC1, 0x74, 0x0F, 0xE8,
                0x26, 0xB3, 0x4C, 0x95, 0xF7, 0x61, 0xDA, 0x18,
                0x83, 0x2E, 0xC9, 0x50, 0xAB, 0x06, 0xF4, 0x3D,
                0x79, 0xE3, 0x14, 0xBD, 0x67, 0x28, 0x9A, 0xD5,
                0x41, 0xFC, 0x0B, 0x72, 0xC7, 0x35, 0x88, 0xE0,
                0x5A, 0xAF, 0x1D, 0x96, 0x43, 0xCB, 0x70, 0x04,
                0xF1, 0x2A, 0xB8, 0x63, 0xDE, 0x19, 0x85, 0x4F,
                0xA3, 0x57, 0xCC, 0x31, 0x0A, 0xED, 0x69, 0xB4
            },
            .valSize = 96,
        },
    };

    HMap* hm = new_hmap();
    VCTEST_ASSERT_TRUE(hm != NULL);
    U64 hmCurrSize = hm->size;

    for (int i = 0; i < testInputsSize; i++)
    {
        // GIVEN
        const U8* const key = inputs[i].key;
        const U64 keySize = inputs[i].keySize;
        U8* val = inputs[i].val;
        const U64 valSize = inputs[i].valSize;

        // WHEN
        hmap_put(hm, key, keySize, val, valSize);
        hmCurrSize++;

        // THEN
        VCTEST_ASSERT_TRUE(hmCurrSize == hm->size);
        MapElem* me = hmap_get(hm, key, keySize);
        VCTEST_ASSERT_TRUE(me->keySize == keySize);
        VCTEST_ASSERT_TRUE( bytesEqual(me->key, key, keySize) );
        VCTEST_ASSERT_TRUE(me->valSize == valSize);
        VCTEST_ASSERT_TRUE( bytesEqual(me->val, val, valSize) );
    }

    // CLEAN-UP
    del_hmap(hm);
}
