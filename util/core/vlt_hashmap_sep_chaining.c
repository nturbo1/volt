#include "vlt_assert.h"
#include "vlt_bytes.h"
#include "vlt_hash.h"
#include "vlt_hashmap.h"

#include <stdio.h>
#include <stdlib.h>

#define LOAD_FACTOR_LIMIT 0.75

#define HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT                        \
    "HashMap bucket entry head %p and size %zu are not consistent."
#define HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT                            \
    "HashMap bucket %p and bucket size %zu are not consistent."

typedef struct SMapElemNode {
    MapElem elem;
    struct SMapElemNode *next;
} MapElemNode;

struct SMapBucketEntry {
    MapElemNode *head;
    U64 size;
};

// Uses FNV-1a hash algorithm
inline static U64 hash(const U8 *const key, const U64 keySize)
{
    return hash_FNV_1a(key, keySize);
}

static void hashmap_delBucketEntryElems(HashMap *const hm, MapBucketEntry *mbe);
static void hashmap_delElemNode(HashMap *const hm, MapElemNode *node);
static MapElemNode *hashmap_NewMapElemNode(HashMap *const hm,
                                           const U8 *const key,
                                           const U64 keySize,
                                           const U8 *const val,
                                           const U64 valSize);
static void rehash(HashMap *hm);
static void putKV(HashMap *const hm, // for accessing unchanged/common funcs and properties
                  MapBucketEntry *buckets,
                  const U64 bucketsSize,
                  const U8 *const key,
                  const U64 keySize,
                  const U8 *const val,
                  const U64 valSize,
                  U64 *elemsSize);

#define DEFAULT_HMAP_BUCKET_SIZE 16

HashMap *new_hashmap(FMemAlloc *const fAllocMem,
                     FMemDealloc *const fDeallocMem,
                     FComparator *const fCompareKeys,
                     FHash *const fHash)
{
    FMemAlloc *const fAllocM = fAllocMem ? fAllocMem : malloc;
    FMemDealloc *const fDeallocM = fDeallocMem ? fDeallocMem : free;
    FHash *const hashFunc = fHash ? fHash : hash;
    FComparator *const fCompareK = fCompareKeys ? fCompareKeys : bytesEqual;

    HashMap *hm = (HashMap *)fAllocM(sizeof(HashMap));
    ASSERT(hm != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "a HashMap obj");
    hm->buckets = NULL;
    hm->buckets = fAllocM(sizeof(MapBucketEntry) * DEFAULT_HMAP_BUCKET_SIZE);
    ASSERT(hm->buckets != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "a HashMap buckets");
    hm->bucketsSize = DEFAULT_HMAP_BUCKET_SIZE;
    for (U64 i = 0; i < hm->bucketsSize; i++) {
        hm->buckets[i].head = NULL;
        hm->buckets[i].size = 0;
    }
    hm->size = 0;
    *(FMemAlloc **)&(hm->fAllocMem) = fAllocM;
    *(FMemDealloc **)&(hm->fDeallocMem) = fDeallocM;
    *(FComparator **)&(hm->fCompareKeys) = fCompareK;
    *(FHash **)&(hm->fHash) = hashFunc;

    return hm;
}

void del_hashmap(HashMap *hm)
{
    if (hm != NULL) {
        for (U64 i = 0; i < hm->bucketsSize; i++)
            hashmap_delBucketEntryElems(hm, hm->buckets + i);
        hm->fDeallocMem(hm->buckets);
        hm->buckets = NULL;
        hm->bucketsSize = 0;
        hm->size = 0;
        hm->fDeallocMem(hm);
    }
}

void hashmap_put(HashMap *const hm,
                 const U8 *const key,
                 const U64 keySize,
                 const U8 *const val,
                 const U64 valSize)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HashMap");
    ASSERT((hm->buckets == NULL) == (hm->bucketsSize == 0),
           HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void *)hm->buckets,
           hm->bucketsSize);

    if ((hm->size / (double)hm->bucketsSize) >= LOAD_FACTOR_LIMIT)
        rehash(hm);

    putKV(hm, hm->buckets, hm->bucketsSize, key, keySize, val, valSize, &(hm->size));
}

MapElem *hashmap_get(const HashMap *const hm, const U8 *const key, const U64 keySize)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HashMap");
    ASSERT((hm->buckets == NULL) == (hm->bucketsSize == 0),
           HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void *)hm->buckets,
           hm->bucketsSize);

    const U64 keyHash = hm->fHash(key, keySize);
    MapBucketEntry *bucketEntry = hm->buckets + (keyHash % hm->bucketsSize);
    ASSERT((bucketEntry->head == NULL) == (bucketEntry->size == 0),
           HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void *)bucketEntry->head,
           bucketEntry->size);

    if (bucketEntry->head == NULL)
        return NULL;

    MapElemNode *curr = bucketEntry->head;
    while (curr) {
        if (curr->elem.keySize == keySize && hm->fCompareKeys(curr->elem.key, key, keySize))
            return (MapElem *)curr;
        curr = curr->next;
    }

    return NULL;
}

void hashmap_del(HashMap *const hm, const U8 *const key, const U64 keySize)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HashMap");
    const U64 keyHash = hm->fHash(key, keySize);
    MapBucketEntry *bucketEntry = hm->buckets + (keyHash % hm->bucketsSize);
    ASSERT((bucketEntry->head == NULL) == (bucketEntry->size == 0),
           HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void *)bucketEntry->head,
           bucketEntry->size);
    MapElemNode *prev = NULL;
    MapElemNode *curr = bucketEntry->head;
    while (curr) {
        if (curr->elem.keySize == keySize && hm->fCompareKeys(curr->elem.key, key, keySize)) {
            if (prev != NULL)
                prev->next = curr->next;
            else
                bucketEntry->head = curr->next;

            hashmap_delElemNode(hm, curr);
            curr = NULL;
            bucketEntry->size--;
            hm->size--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// =========================================================================================
// ================================ STATIC/PRIVATE/INTERNAL ================================
// =========================================================================================

static void hashmap_delBucketEntryElems(HashMap *const hm, MapBucketEntry *mbe)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HashMap");
    if (mbe != NULL) {
        ASSERT((mbe->head == NULL) == (mbe->size == 0),
               HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
               (void *)mbe->head,
               mbe->size);
        MapElemNode *prev = NULL;
        MapElemNode *curr = mbe->head;
        while (curr) {
            prev = curr;
            curr = curr->next;
            hashmap_delElemNode(hm, prev);
            mbe->size--;
            prev = NULL;
        }
        mbe->head = NULL;
        ASSERT(mbe->size == 0, "HashMap bucket entry size doesn't match the nodes count.");
    }
}

static void hashmap_delElemNode(HashMap *const hm, MapElemNode *node)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HashMap");
    if (node != NULL) {
        hm->fDeallocMem((U8 *)node->elem.key);
        hm->fDeallocMem(node->elem.val);
        node->elem.val = NULL;
        node->elem.valSize = 0;
        hm->fDeallocMem(node);
    }
}

static void putKV(HashMap *const hm, // for accessing unchanged/common funcs and properties
                  MapBucketEntry *buckets,
                  const U64 bucketsSize,
                  const U8 *const key,
                  const U64 keySize,
                  const U8 *const val,
                  const U64 valSize,
                  U64 *elemsSize)
{
    ASSERT(buckets != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "MapBucketEntry");

    const U64 keyHash = hm->fHash(key, keySize);
    MapBucketEntry *bucketEntry = buckets + (keyHash % bucketsSize);

    MapElemNode *prev = NULL;
    MapElemNode *curr = bucketEntry->head;
    while (curr) {
        if (curr->elem.keySize == keySize && hm->fCompareKeys(curr->elem.key, key, keySize)) {
            hm->fDeallocMem(curr->elem.val);
            curr->elem.val = copyBytesToNew(val, valSize);
            curr->elem.valSize = valSize;
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    MapElemNode *newElemNode = hashmap_NewMapElemNode(hm, key, keySize, val, valSize);
    if (prev == NULL) // means the bucket is empty -> bucketEntry->head == NULL
        bucketEntry->head = newElemNode;
    else
        prev->next = newElemNode;
    bucketEntry->size++;
    (*elemsSize)++;
}

static void rehash(HashMap *hm)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HashMap");

    const U64 newBucketsSize = hm->bucketsSize * 2;
    MapBucketEntry *newBuckets
        = (MapBucketEntry *)hm->fAllocMem(sizeof(MapBucketEntry) * newBucketsSize);
    ASSERT(newBuckets != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "HashMap new buckets array");
    for (U64 i = 0; i < newBucketsSize; i++) {
        newBuckets[i].head = NULL;
        newBuckets[i].size = 0;
    }

    U64 elemsSize = 0;
    for (U64 i = 0; i < hm->bucketsSize; i++) {
        MapBucketEntry *mbe = hm->buckets + i;
        ASSERT((mbe->head == NULL) == (mbe->size == 0),
               HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
               (void *)mbe->head,
               mbe->size);

        if (mbe->size == 0)
            continue;

        MapElemNode *curr = mbe->head;
        while (curr) {
            putKV(hm,
                  newBuckets,
                  newBucketsSize,
                  curr->elem.key,
                  curr->elem.keySize,
                  curr->elem.val,
                  curr->elem.valSize,
                  &elemsSize);
            curr = curr->next;
        }
    }
    ASSERT(elemsSize == hm->size,
           "Hashmap elements count during rehash MUST match the hashmap size.");

    for (U64 i = 0; i < hm->bucketsSize; i++)
        hashmap_delBucketEntryElems(hm, hm->buckets + i);
    hm->fDeallocMem(hm->buckets);
    hm->buckets = newBuckets;
    hm->bucketsSize = newBucketsSize;
}

static MapElemNode *hashmap_NewMapElemNode(HashMap *const hm,
                                           const U8 *const key,
                                           const U64 keySize,
                                           const U8 *const val,
                                           const U64 valSize)
{
    ASSERT(hm, NULL_POINTER_ERROR_MSG_FORMAT, "HashMap");
    MapElemNode *newElemNode = hm->fAllocMem(sizeof(MapElemNode));
    ASSERT(newElemNode != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "a MapElemNode obj");
    *((U8 **)&(newElemNode->elem.key)) = copyBytesToNew(key, keySize);
    *((U64 *)&(newElemNode->elem.keySize)) = keySize;
    newElemNode->elem.val = copyBytesToNew(val, valSize);
    newElemNode->elem.valSize = valSize;
    newElemNode->next = NULL;

    return newElemNode;
}

void hashmap_print(const HashMap *const hm,
                   FILE *const outFile,
                   HashMapElemKeyFPrint printKey,
                   HashMapElemValFPrint printVal)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HashMap");
    FILE *out = stdout;
    if (outFile != NULL)
        out = outFile;

    fprintf(out, "{size: %zu,\n", hm->size);
    fprintf(out, " buckets:\n[");
    for (U64 i = 0; i < hm->bucketsSize; i++) {
        MapBucketEntry *mbe = hm->buckets + i;
        ASSERT((mbe->head == NULL) == (mbe->size == 0),
               HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
               (void *)mbe->head,
               mbe->size);
        fprintf(out, "[%zu] size=%zu head->", i, mbe->size);
        if (mbe->size > 0) {
            MapElemNode *curr = mbe->head;
            while (curr) {
                fprintf(out, "{key = ");
                printKey(out, curr->elem.key);
                fprintf(out, ", val = ");
                printVal(out, curr->elem.val);
                fprintf(out, "}->");
                curr = curr->next;
            }
            fprintf(out, "NULL");
        } else {
            fprintf(out, "NULL");
        }
        if (i == hm->bucketsSize - 1)
            fprintf(out, "]");
        else
            fprintf(out, "\n ");
    }
    fprintf(out, "}\n");
}
