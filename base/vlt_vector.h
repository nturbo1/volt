#ifndef BASE_VLT_VECTOR_H
#define BASE_VLT_VECTOR_H

#include "base.h"

typedef struct
{
    U64 len;
    U64 cap;
    const U64 elem_size;
    U8* buf;
}
Vec;

Vec* new_vec(const U64 len, const U64 cap, const U64 elem_size);

// Deletes a given Vecor object resources.
// It's the programmer's responsibility to set the deleted Vecor
// object pointer to NULL afterwards!
void del_vec(Vec* vt);

// Inserts given element bytes at a given index.
// If the element bytes size doesn't match the Vecor element size,
// then it may or may not abort the program.
void vec_insert(Vec* const vt,
                const U64 idx,
                const U8* const elem_bytes,
                const U64 elem_size);

// Appends given element bytes to the end of the Vecor.
// - If the element bytes size doesn't match the Vecor element size,
//   then it may or may not abort the program.
// - If the vector is empty, then it returns NULL.
void vec_push(Vec* const vt,
              const U64 elem_size,
              const U8* const elem_bytes);

// Removes element bytes at the end of the Vecor and returns a
// pointer to the removed element bytes.
U8* vec_pop(Vec* const vt);

// Returns a pointer to element bytes at a given index.
U8* vec_get(Vec* const vt, const U64 idx);

#endif // BASE_VLT_VECTOR_H
