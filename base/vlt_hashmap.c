#include "vlt_hashmap.h"
#include "base.h"
#include "vlt_assert.h"

#include <stdlib.h>

#define LOAD_FACTOR_LIMIT 0.75

#define HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT "HMap bucket entry head %p and size %zu are not consistent."
#define HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT "HMap bucket %p and bucket size %zu are not consistent."

static U64 hash(const U8* const key, const U64 keySize);
static void hmap_delBucketEntryElems(MapBucketEntry* mbe);
static void hmap_delElemNode(MapElemNode* node);
static MapElemNode* newMapElemNode(const U8* const key,
                                   const U64 keySize,
                                   const U8* const val,
                                   const U64 valSize);
static void rehash(HMap* hm);
static void putKV(MapBucketEntry* buckets,
                  const U64 bucketsSize,
                  const U8* const key,
                  const U64 keySize,
                  const U8* const val,
                  const U64 valSize,
                  U64* elemsSize);

#define DEFAULT_HMAP_BUCKET_SIZE 16

HMap* new_hmap()
{
    HMap* hm = (HMap*) malloc(sizeof(HMap));
    ASSERT(hm != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "a HMap obj");
    hm->buckets = NULL;
    hm->buckets = malloc(sizeof(MapBucketEntry) * DEFAULT_HMAP_BUCKET_SIZE);
    ASSERT(hm->buckets != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "a HMap buckets");
    hm->bucketsSize = DEFAULT_HMAP_BUCKET_SIZE;
    for (U64 i = 0; i < hm->bucketsSize; i++)
    {
        hm->buckets[i].head = NULL;
        hm->buckets[i].size = 0;
    }
    hm->size = 0;

    return hm;
}

void del_hmap(HMap* hm)
{
    if (hm != NULL)
    {
        for (U64 i = 0; i < hm->bucketsSize; i++)
            hmap_delBucketEntryElems(hm->buckets + i);
        free(hm->buckets);
        hm->buckets = NULL;
        hm->bucketsSize = 0;
        hm->size = 0;
        free(hm);
    }
}

void hmap_put(HMap* const hm,
              const U8* const key,
              const U64 keySize,
              const U8* const val,
              const U64 valSize)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HMap");
    ASSERT((hm->buckets == NULL) == (hm->bucketsSize == 0),
           HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void*) hm->buckets,
           hm->bucketsSize);

    if ( (hm->size / hm->bucketsSize) >= LOAD_FACTOR_LIMIT )
        rehash(hm);

    putKV(hm->buckets, hm->bucketsSize, key, keySize, val, valSize, &(hm->size));
}

MapElem* hmap_get(const HMap* const hm, const U8* const key, const U64 keySize)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HMap");
    ASSERT((hm->buckets == NULL) == (hm->bucketsSize == 0),
           HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void*) hm->buckets,
           hm->bucketsSize);

    const U64 keyHash = hash(key, keySize);
    MapBucketEntry* bucketEntry = hm->buckets + (keyHash % hm->bucketsSize);
    ASSERT((bucketEntry->head == NULL) == (bucketEntry->size == 0),
           HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void*) bucketEntry->head,
           bucketEntry->size);

    if (bucketEntry->head == NULL)
        return NULL;

    MapElemNode* curr = bucketEntry->head;
    while (curr)
    {
        if (curr->elem.keySize == keySize && bytesEqual(curr->elem.key, key, keySize))
            return (MapElem*) curr;
        curr = curr->next;
    }

    return NULL;
}

void hmap_delete(HMap* const hm, const U8* const key, const U64 keySize)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HMap");
    const U64 keyHash = hash(key, keySize);
    MapBucketEntry* bucketEntry = hm->buckets + (keyHash % hm->bucketsSize);
    ASSERT((bucketEntry->head == NULL) == (bucketEntry->size == 0),
           HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void*) bucketEntry->head,
           bucketEntry->size);
    MapElemNode* prev = NULL;
    MapElemNode* curr = bucketEntry->head;
    while (curr)
    {
        if (curr->elem.keySize == keySize && bytesEqual(curr->elem.key, key, keySize))
        {
            if (prev != NULL)
                prev->next = curr->next;
            else
                bucketEntry->head = curr->next;

            hmap_delElemNode(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// =========================================================================================
// ================================ STATIC/PRIVATE/INTERNAL ================================
// =========================================================================================

// Parameters for FNV Hash algorithms
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Uses FNV-1a hash algorithm
static U64 hash(const U8* const key, const U64 keySize) {
    U64 hashval = FNV_OFFSET;

    for (U64 i = 0; i < keySize; i++)
    {
        hashval ^= (U64)(U8) key[i];
        hashval *= FNV_PRIME;
    }

    return hashval;
}

static void hmap_delBucketEntryElems(MapBucketEntry* mbe)
{
    if (mbe != NULL)
    {
        ASSERT((mbe->head == NULL) == (mbe->size == 0),
               HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
               (void*) mbe->head,
               mbe->size);
        MapElemNode* prev = NULL;
        MapElemNode* curr = mbe->head;
        while (curr)
        {
            prev = curr;
            curr = curr->next;
            hmap_delElemNode(prev);
            mbe->size--;
            prev = NULL;
        }
        mbe->head = NULL;
        ASSERT(mbe->size == 0, "HMap bucket entry size doesn't match the nodes count.");
    }
}

static void hmap_delElemNode(MapElemNode* node)
{
    if (node != NULL)
    {
        free((U8*) node->elem.key);
        free(node->elem.val);
        node->elem.val = NULL;
        node->elem.valSize = 0;
        free(node);
    }
}

static void putKV(MapBucketEntry* buckets,
                  const U64 bucketsSize,
                  const U8* const key,
                  const U64 keySize,
                  const U8* const val,
                  const U64 valSize,
                  U64* elemsSize)
{
    ASSERT(buckets != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "MapBucketEntry");

    const U64 keyHash = hash(key, keySize);
    MapBucketEntry* bucketEntry = buckets + (keyHash % bucketsSize);

    MapElemNode* prev = NULL;
    MapElemNode* curr = bucketEntry->head;
    while (curr)
    {
        if (curr->elem.keySize == keySize && bytesEqual(curr->elem.key, key, keySize))
        {
            free(curr->elem.val);
            curr->elem.val = copyBytesToNew(val, valSize);
            curr->elem.valSize = valSize;
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    MapElemNode* newElemNode = newMapElemNode(key, keySize, val, valSize);
    if (prev == NULL) // means the bucket is empty -> bucketEntry->head == NULL
        bucketEntry->head = newElemNode;
    else
        prev->next = newElemNode;
    bucketEntry->size++;
    (*elemsSize)++;
}

static void rehash(HMap* hm)
{
    ASSERT(hm != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "HMap");

    const U64 newBucketsSize = hm->bucketsSize * 2;
    MapBucketEntry* newBuckets = (MapBucketEntry*) malloc(sizeof(MapBucketEntry) * newBucketsSize);
    ASSERT(newBuckets != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "HMap new buckets array");
    for (U64 i = 0; i < newBucketsSize; i++)
    {
        newBuckets[i].head = NULL;
        newBuckets[i].size = 0;
    }

    U64 elemsSize = 0;
    for (U64 i = 0; i < hm->bucketsSize; i++)
    {
        MapBucketEntry* mbe = hm->buckets + i;
        ASSERT((mbe->head == NULL) == (mbe->size == 0),
               HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
               (void*) mbe->head,
               mbe->size);

        if (mbe->size == 0)
            continue;

        MapElemNode* curr = mbe->head;
        while (curr)
        {
            putKV(newBuckets,
                  newBucketsSize,
                  curr->elem.key,
                  curr->elem.keySize,
                  curr->elem.val,
                  curr->elem.valSize,
                  &elemsSize);
            curr = curr->next;
        }
    }
    ASSERT(elemsSize == hm->size, "Hashmap elements count during rehash MUST match the hashmap size.");

    for (U64 i = 0; i < hm->bucketsSize; i++)
        hmap_delBucketEntryElems(hm->buckets + i);
    free(hm->buckets);
    hm->buckets = newBuckets;
    hm->bucketsSize = newBucketsSize;
}

static MapElemNode* newMapElemNode(const U8* const key,
                                   const U64 keySize,
                                   const U8* const val,
                                   const U64 valSize)
{
    MapElemNode* newElemNode = malloc(sizeof(MapElemNode));
    ASSERT(newElemNode != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "a MapElemNode obj");
    *( (U8**) &(newElemNode->elem.key) ) = copyBytesToNew(key, keySize);
    *( (U64*) &(newElemNode->elem.keySize) ) = keySize;
    newElemNode->elem.val = copyBytesToNew(val, valSize);
    newElemNode->elem.valSize = valSize;
    newElemNode->next = NULL;

    return newElemNode;
}
