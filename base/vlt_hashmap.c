#include "vlt_hashmap.h"
#include "base.h"
#include "vlt_assert.h"

#include <stdlib.h>

#define NULL_HMAP_POINTER_ERROR_MSG "NULL HMap pointer was passed!"
#define HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT "HMap bucket entry head %p and size %zu are not consistent."
#define HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT "HMap bucket %p and bucket size %zu are not consistent."

static U64 hash(const U8* const key, const U64 keySize);
static void hmap_delBucketEntry(MapBucketEntry* mbe);
static void hmap_delElemNode(MapElemNode* node);
static MapElemNode* new_mapElemNode(const U8* const key, const U64 keySize, U8* const val, const U64 valSize);
static bool keyEqual(const U8* const key1, const U8* const key2, const U64 keySize);

HMap* new_hmap()
{
    HMap* hm = (HMap*) malloc(sizeof(HMap));
    assert(hm != NULL, "Failed to allocate memory for a HMap object.");
    hm->bucket = NULL;
    hm->bucketSize = 0;
    hm->size = 0;

    return hm;
}

void del_hmap(HMap* hm)
{
    if (hm != NULL)
    {
        for (U64 i = 0; i < hm->bucketSize; i++)
            hmap_delBucketEntry(hm->bucket + i);
        hm->bucket = NULL;
        hm->bucketSize = 0;
        hm->size = 0;
        free(hm);
    }
}

void hmap_put(HMap* const hm,
              const U8* const key,
              const U64 keySize,
              U8* val,
              const U64 valSize)
{
    assert(hm != NULL, NULL_HMAP_POINTER_ERROR_MSG);
    assert((hm->bucket == NULL) == (hm->bucketSize == 0),
           HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void*) hm->bucket,
           hm->bucketSize);
    // TODO: CHECK THE LOAD FACTOR AND EXPAND MAP IF NECESSARY!!!

    MapElem* elem = hmap_get(hm, key, keySize);
    if (elem != NULL) // update an existing element value
    {
        free(elem->val);
        elem->val = val;
        elem->valSize = valSize;
    }
    else // add a new element
    {
        MapElemNode* newElemNode = new_mapElemNode(key, keySize, val, valSize);
        const U64 keyHash = hash(key, keySize);
        MapBucketEntry* bucketEntry = hm->bucket + (keyHash % hm->bucketSize);
        if (bucketEntry->head == NULL)
        {
            bucketEntry->head = newElemNode;
        }
        else
        {
            MapElemNode* curr = bucketEntry->head;
            while (curr->next)
                curr = curr->next;
            curr->next = newElemNode;
        }
        bucketEntry->size++;
    }
}

MapElem* hmap_get(const HMap* const hm, const U8* const key, const U64 keySize)
{
    assert(hm != NULL, NULL_HMAP_POINTER_ERROR_MSG);
    assert((hm->bucket == NULL) == (hm->bucketSize == 0),
           HMAP_BUCKET_AND_BUCKET_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void*) hm->bucket,
           hm->bucketSize);

    const U64 keyHash = hash(key, keySize);
    MapBucketEntry* bucketEntry = hm->bucket + (keyHash % hm->bucketSize);
    assert((bucketEntry->head == NULL) == (bucketEntry->size == 0),
           HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void*) bucketEntry->head,
           bucketEntry->size);

    if (bucketEntry->head == NULL)
        return NULL;

    MapElemNode* curr = bucketEntry->head;
    while (curr)
    {
        if (curr->elem.keySize == keySize && keyEqual(curr->elem.key, key, keySize))
            return (MapElem*) curr;
        curr = curr->next;
    }

    return NULL;
}

void hmap_delete(HMap* const hm, const U8* const key, const U64 keySize)
{
    assert(hm != NULL, NULL_HMAP_POINTER_ERROR_MSG);
    const U64 keyHash = hash(key, keySize);
    MapBucketEntry* bucketEntry = hm->bucket + (keyHash % hm->bucketSize);
    assert((bucketEntry->head == NULL) == (bucketEntry->size == 0),
           HMAP_BUCKET_ENTRY_HEAD_AND_SIZE_ARE_NOT_CONSISTENT_ERROR_MSG_FORMAT,
           (void*) bucketEntry->head,
           bucketEntry->size);
    MapElemNode* prev = NULL;
    MapElemNode* curr = bucketEntry->head;
    while (curr)
    {
        if (curr->elem.keySize == keySize && keyEqual(curr->elem.key, key, keySize))
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

static void hmap_delBucketEntry(MapBucketEntry* mbe)
{
    if (mbe != NULL)
    {
        assert((mbe->head == NULL) == (mbe->size == 0),
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
        assert(mbe->size == 0, "HMap bucket entry size doesn't match the nodes count.");
        mbe->size = 0;
        free(mbe);
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

static MapElemNode* new_mapElemNode(const U8* const key, const U64 keySize, U8* const val, const U64 valSize)
{
    MapElemNode* newElemNode = malloc(sizeof(MapElemNode));
    assert(newElemNode != NULL, "Failed to allocate memory for a MapElemNode object.");
    U8* elemKey = *((U8**) &newElemNode->elem.key);
    for (U64 i = 0; i < keySize; i++)
        elemKey[i] = key[i];
    *((U64*) &newElemNode->elem.keySize) = keySize;
    newElemNode->elem.val = val;
    newElemNode->elem.valSize = valSize;
    newElemNode->next = NULL;

    return newElemNode;
}

static bool keyEqual(const U8* const key1, const U8* const key2, const U64 keySize)
{
    for (U64 i = 0; i < keySize; i++)
        if (key1[i] != key2[i])
            return false;

    return true;
}
