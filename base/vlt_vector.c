#include "vlt_vector.h"
#include "vlt_assert.h"

#include <stdlib.h>
#include <stddef.h>

#define VEC_DEFAULT_CAPACITY_SIZE 16
#define NULL_VEC_POINTER_ERROR_MSG "NULL Vector pointer was passed"
#define VEC_INDEX_OUT_OF_BOUNDS_ERROR_MSG "Vector index out of bounds."

static void vec_expand(Vec* vt);

Vec* new_vec(const U64 len, const U64 cap, const U64 elemSize)
{
    assert(elemSize > 0, "Vector elem size can't be 0!");
    assert(cap >= len, "Not enough Vec capacity provided.");
    assert(elemSize <= U32_MAX, "Vec element size is too big!");
    assert(len == 0 || len <= U64_MAX / elemSize, "Vec length is too big!");
    assert(cap == 0 || cap <= U64_MAX / elemSize, "Vec capacity is too big!");

    Vec* vt = (Vec*) malloc(sizeof(Vec));
    assert(vt != NULL, "Failed to allocate memory for a Vec object.");
    U8* buf = NULL;
    if (cap > 0)
    {
        buf = (U8*) malloc(elemSize * cap);
        assert(buf != NULL, "Failed to allocate memory for a Vec buffer.");
    }

    vt->len = len;
    vt->cap = cap;
    *((U64*) &(vt->elemSize)) = elemSize;
    vt->buf = buf;

    return vt;
}

void del_vec(Vec* vt)
{
    if (vt == NULL)
        return;

    free(vt->buf);
    free(vt);
}

void vec_insert(Vec* const vt,
                const U64 idx,
                const U8* const elem_bytes,
                const U64 elemSize)
{
    assert(vt != NULL, NULL_VEC_POINTER_ERROR_MSG);
    assert(vt->elemSize == elemSize, "Passed elem size doesn't match the vector elemSize.");
    assert(idx < vt->len, VEC_INDEX_OUT_OF_BOUNDS_ERROR_MSG);
    assert(vt->buf != NULL, "Can't insert at an index to an empty Vector.");

    for (U64 i = 0; i < elemSize; i++)
        vt->buf[idx + i] = elem_bytes[i];
}

void vec_push(Vec* const vt,
              const U64 elemSize,
              const U8* const elem_bytes)
{
    assert(vt != NULL, NULL_VEC_POINTER_ERROR_MSG);
    assert(vt->elemSize == elemSize, "Passed elem size doesn't match the vector elemSize.");
    assert_dbg(vt->len <= vt->cap, "Vector length can't be bigger than the capacity.");
    if (vt->len == vt->cap)
        vec_expand(vt);

    vec_insert(vt, elemSize, elem_bytes, vt->len);
    vt->len++;
}

U8* vec_pop(Vec* const vt)
{
    assert(vt != NULL, NULL_VEC_POINTER_ERROR_MSG);
    if (vt->len == 0)
        return NULL;

    U8* last_elem_bytes_copy = (U8*) malloc(vt->elemSize);
    assert(last_elem_bytes_copy != NULL, "Failed to allocate memory for a vector element.");
    U8* last_elem_bytes = vt->buf + (vt->elemSize * (vt->len - 1));
    for (U64 i = 0; i < vt->elemSize; i++)
        last_elem_bytes_copy[i] = last_elem_bytes[i];
    vt->len--;

    return last_elem_bytes_copy;
}

U8* vec_get(Vec* const vt, const U64 idx)
{
    assert(vt != NULL, NULL_VEC_POINTER_ERROR_MSG);
    assert(idx < vt->len, VEC_INDEX_OUT_OF_BOUNDS_ERROR_MSG);

    return vt->buf + (vt->elemSize * idx);
}

static void vec_expand(Vec* vt)
{
    assert(vt != NULL, NULL_VEC_POINTER_ERROR_MSG);
    if (vt->buf == NULL)
    {
        assert(vt->cap == 0, "Vector capacity MUST be 0 when the buffer is NULL.");
        assert(vt->len == 0, "Vector length MUST be 0 when the buffer is NULL.");
        vt->buf = (U8*) malloc(VEC_DEFAULT_CAPACITY_SIZE);
        assert(vt->buf != NULL, "Failed to allocate memory to Vector buffer.");
        vt->cap = VEC_DEFAULT_CAPACITY_SIZE;
        return;
    }

    U64 new_cap = vt->cap * 2;
    U8* new_buf = (U8*) malloc(new_cap);
    assert(new_buf != NULL, "Failed to allocate memory to Vector new expanded buffer.");
    // copy the raw bytes to the new buffer
    for (U64 i = 0; i < vt->len * vt->elemSize; i++)
        new_buf[i] = vt->buf[i];

    free(vt->buf);
    vt->buf = new_buf;
    vt->cap = new_cap;
}
