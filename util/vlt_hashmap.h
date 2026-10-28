#ifndef BASE_VLT_HASHMAP_H
#define BASE_VLT_HASHMAP_H

#include "config.h"
#include "vlt_assert.h"
#include "vlt_container.h"
#include "vlt_memory.h"

#include <stdio.h>

typedef struct SMapElem {
    const U8 *const key;
    const U64 keySize;
    U8 *val;
    U64 valSize;
} MapElem;

typedef struct SMapBucketEntry MapBucketEntry;
struct SHashMap {
    MapBucketEntry *buckets;
    U64 bucketsSize;
    U64 size;
    FMemAlloc *const fAllocMem;
    FMemDealloc *const fDeallocMem;
    FComparator *const fCompareKeys;
    FHash *const fHash;
};
typedef struct SHashMap HashMap;

HashMap *new_hashmap(FMemAlloc *const fAllocMem,
                     FMemDealloc *const fDeallocMem,
                     FComparator *const fCompareKeys,
                     FHash *const fHash);

void del_hashmap(HashMap *hm);

void hashmap_put(HashMap *const hm,
                 const U8 *const key,
                 const U64 keySize,
                 const U8 *const val,
                 const U64 valSize);

MapElem *hashmap_get(const HashMap *const hm, const U8 *const key, const U64 keySize);
void hashmap_del(HashMap *const hm, const U8 *const key, const U64 keySize);

#define GEN_HASHMAP_OF(K, V)                                                                       \
    inline void hashmap##K##V##_put(HashMap *const hm, const K *const key, const V *const val)     \
    {                                                                                              \
        hashmap_put(hm, (const U8 *const)key, sizeof(K), (const U8 *const)val, sizeof(V));         \
    }                                                                                              \
    inline const V *hashmap##K##V##_get(const HashMap *const hm, const K *const key)               \
    {                                                                                              \
        MapElem *elem = hashmap_get(hm, (const U8 *const)key, sizeof(K));                          \
        if (elem) {                                                                                \
            ASSERT_DBG(elem->key, "Existing map element key must not be null.");                   \
            ASSERT_DBG(elem->keySize == sizeof(K),                                                 \
                       "Map element key size doesn't match the key type size.");                   \
            ASSERT_DBG(elem->valSize == sizeof(V),                                                 \
                       "Map element value size doesn't match the value type size.");               \
            return (EToken *)elem->val;                                                            \
        }                                                                                          \
    }                                                                                              \
    inline void hashmap##K##V##_del(HashMap *const hm, const K *const key)                         \
    {                                                                                              \
        hashmap_del(hm, (const U8 *const)key, sizeof(K));                                          \
    }

typedef void (*HashMapElemKeyFPrint)(FILE *const out, const U8 *const key);
typedef void (*HashMapElemValFPrint)(FILE *const out, const U8 *const val);

/*
 * Prints out the contents of a given hashmap to a given file.
 *
 * Mostly used for debugging purposes.
 */
void hashmap_print(const HashMap *const hm,
                   FILE *const outFile,
                   HashMapElemKeyFPrint printKey,
                   HashMapElemValFPrint printVal);

#endif // BASE_VLT_HASHMAP_H
