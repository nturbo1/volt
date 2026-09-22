#ifndef BASE_VLT_HASHMAP_H
#define BASE_VLT_HASHMAP_H

#include "base.h"

#include <stdio.h>

typedef struct
{
    const U8* const key;
    const U64 keySize;
    U8* val;
    U64 valSize;
}
MapElem;

typedef struct MapElemNode
{
    MapElem elem;
    struct MapElemNode* next;
}
MapElemNode;

typedef struct
{
    MapElemNode* head;
    U64 size;
}
MapBucketEntry;

typedef struct
{
    MapBucketEntry* buckets;
    U64 bucketsSize;
    U64 size;
}
HMap;

HMap* new_hmap();

// Deletes the resources of a given HMap object.
//
// It's the programmer's responsibility to set the HMap pointer to NULL
// immediately after the deletion.
void del_hmap(HMap* hm);

void hmap_put(HMap* const hm,
              const U8* const key,
              const U64 keySize,
              const U8* const val,
              const U64 valSize);

MapElem* hmap_get(const HMap* const hm, const U8* const key, const U64 keySize);
void hmap_del(HMap* const hm, const U8* const key, const U64 keySize);

typedef void (*HMapElemKeyFPrint)(FILE* const out, const U8* const key);
typedef void (*HMapElemValFPrint)(FILE* const out, const U8* const val);

/*
 * Prints out the contents of a given hashmap to a given file.
 *
 * Mostly used for debugging purposes.
 */
void hmap_print(const HMap* const hm,
                FILE* const outFile,
                HMapElemKeyFPrint printKey,
                HMapElemValFPrint printVal);

#endif // BASE_VLT_HASHMAP_H
