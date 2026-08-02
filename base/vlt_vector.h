#ifndef BASE_VLT_VECTOR_H
#define BASE_VLT_VECTOR_H

#include "base.h"

typedef struct
{
    U64 len;
    U64 cap;
    U64 elem_size;
    U8* buf;
}
Vect;

Vect* new_vect(const U64 len, const U64 cap, const U64 elem_size);

// Deletes a given Vector object resources.
// It's the programmer's responsibility to set the deleted Vector
// object pointer to NULL afterwards!
Bool del_vect(Vect* vt);

// Inserts given element bytes at a given index.
// If the element bytes size doesn't match the Vector element size,
// then it returns false and may or may not abort the program,
// otherwise it returns true.
Bool vect_insert(const Vect* const vt,
                 const U64 elem_size,
                 const U8* const elem_bytes,
                 const U64 idx);

// Appends given element bytes to the end of the Vector. 
// If the element bytes size doesn't match the Vector element size,
// then it returns false and may or may not abort the program,
// otherwise it returns true.
Bool vect_push(const Vect* const vt,
               const U64 elem_size,
               const U8* const elem_bytes);

// Removes element bytes at the end of the Vector and returns a
// pointer to the removed element bytes.
void* vect_pop(const Vect* const vt);

#endif // BASE_VLT_VECTOR_H
