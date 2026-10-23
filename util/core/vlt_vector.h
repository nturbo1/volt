#ifndef BASE_VLT_VECTOR_H
#define BASE_VLT_VECTOR_H

#include "base.h"

typedef struct {
    U64 len;
    U64 cap;
    const U64 elemSize;
    U8 *buf;
} Vec;

Vec *new_vec(const U64 len, const U64 cap, const U64 elemSize);

// Deletes a given Vecor object resources.
// It's the programmer's responsibility to set the deleted Vecor
// object pointer to NULL afterwards!
void del_vec(Vec *vt);

// Inserts given element bytes at a given index.
// If the element bytes size doesn't match the Vecor element size,
// then it may or may not abort the program.
void vec_insert(Vec *const vt, const U64 idx, const U8 *const elemBytes, const U64 elemSize);

// Appends given element bytes to the end of the Vecor.
// - If the element bytes size doesn't match the Vecor element size,
//   then it may or may not abort the program.
// - If the vector is empty, then it returns NULL.
void vec_push(Vec *const vt, const U64 elemSize, const U8 *const elemBytes);

// Removes element bytes at the end of the Vecor and returns a
// pointer to the removed element bytes.
U8 *vec_pop(Vec *const vt);

// Returns a pointer to element bytes at a given index.
U8 *vec_get(Vec *const vt, const U64 idx);

#define GEN_VECTOR_OF(T)                                                                           \
    inline Vec *new_vec##T(const U64 len, const U64 cap)                                           \
    {                                                                                              \
        return new_vec(len, cap, sizeof(T));                                                       \
    }                                                                                              \
    inline void vec##T_insert(Vec *const vt, const U64 idx, const T *const elem)                   \
    {                                                                                              \
        vec_insert(vt, idx, (const U8 *const)elem, sizeof(T));                                     \
    }                                                                                              \
    inline void vec##T_push(Vec *const vt, const T *const elem)                                    \
    {                                                                                              \
        vec_push(vt, sizeof(T), (const U8 *const)elem);                                            \
    }                                                                                              \
    inline T *vec##T_pop(Vec *const vt)                                                            \
    {                                                                                              \
        return (T *)vec_pop(vt);                                                                   \
    }                                                                                              \
    inline T *vec##T_get(Vec *const vt, const U64 idx)                                             \
    {                                                                                              \
        return (T *)vec_get(vt, idx);                                                              \
    }

#endif // BASE_VLT_VECTOR_H
