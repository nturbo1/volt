#include "vlt_vector.h"
#include "vlt_assert.h"

#include <stdlib.h>

#define VEC_DEFAULT_CAPACITY_SIZE 16

static void vec_expand(Vec* vt);

Vec* new_vec(const U64 len, const U64 cap, const U64 elem_size)
{
    assert(elem_size > 0, "Vector elem size can't be 0!");
    assert(cap >= len, "Not enough Vec capacity provided.");
    assert(elem_size <= U32_MAX, "Vec element size is too big!");
    assert(len == 0 || len <= U64_MAX / elem_size, "Vec length is too big!");
    assert(cap == 0 || cap <= U64_MAX / elem_size, "Vec capacity is too big!");

    Vec* vt = (Vec*) malloc(sizeof(Vec));
    assert(vt != NULL, "Failed to allocate memory for a Vec object.");
    U8* buf = NULL;
    if (cap > 0)
    {
        buf = (U8*) malloc(elem_size * cap);
        assert(buf != NULL, "Failed to allocate memory for a Vec buffer.");
    }

    vt->len = len;
    vt->cap = cap;
    *((U64*) &(vt->elem_size)) = elem_size;
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
                const U64 elem_size,
                const U8* const elem_bytes,
                const U64 idx)
{
    assert(vt != NULL, "NULL Vector pointer was passed.");
    assert(vt->elem_size == elem_size, "Passed elem size doesn't match the vector elem_size.");
    assert(idx < vt->len, "Vector index out of bounds.");
    assert(vt->buf != NULL, "Can't insert at an index to an empty Vector.");

    for (U64 i = 0; i < elem_size; i++)
        vt->buf[idx + i] = elem_bytes[i];
}

void vec_push(Vec* const vt,
              const U64 elem_size,
              const U8* const elem_bytes)
{
    assert(vt != NULL, "NULL Vector pointer was passed.");
    assert(vt->elem_size == elem_size, "Passed elem size doesn't match the vector elem_size.");
    assert_dbg(vt->len <= vt->cap, "Vector length can't be bigger than the capacity.");
    if (vt->len == vt->cap)
        vec_expand(vt);

    vec_insert(vt, elem_size, elem_bytes, vt->len);
    vt->len++;
}

U8* vec_pop(Vec* const vt)
{
    assert(vt != NULL, "NULL Vector pointer was passed.");
    if (vt->len == 0)
        return NULL;

    U8* last_elem_bytes_copy = (U8*) malloc(vt->elem_size);
    assert(last_elem_bytes_copy != NULL, "Failed to allocate memory for a vector element.");
    U8* last_elem_bytes = vt->buf + (vt->elem_size * (vt->len - 1));
    for (U64 i = 0; i < vt->elem_size; i++)
        last_elem_bytes_copy[i] = last_elem_bytes[i];
    vt->len--;

    return last_elem_bytes_copy;
}

void vec_expand(Vec* vt)
{
    assert(vt != NULL, "NULL Vector pointer was passed.");
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
    for (U64 i = 0; i < vt->len * vt->elem_size; i++)
        new_buf[i] = vt->buf[i];

    free(vt->buf);
    vt->buf = new_buf;
    vt->cap = new_cap;
}
