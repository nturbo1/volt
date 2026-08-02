#include "vlt_vector.h"
#include "vlt_assert.h"

#include <stdlib.h>

Vect* new_vect(const U64 len, const U64 cap, const U64 elem_size)
{
    assert(cap >= len, "Not enough Vect capacity provided.");
    assert(elem_size <= U32_MAX, "Vect element size is too big!");
    assert(len == 0 || len <= U64_MAX / elem_size, "Vect length is too big!");
    assert(cap == 0 || cap <= U64_MAX / elem_size, "Vect capacity is too big!");

    Vect* vt = (Vect*) malloc(sizeof(Vect));
    assert(vt != NULL, "Failed to allocate memory for a Vect object.");
    U8* buf = (U8*)malloc(elem_size * cap);
    assert(buf != NULL, "Failed to allocate memory for a Vect buffer.");

    vt->len = len;
    vt->cap = cap;
    vt->elem_size = elem_size;
    vt->buf = buf;

    return vt;
}

Bool del_vect(Vect* vt)
{
    if (vt == NULL)
        return true;

    free(vt->buf);
    free(vt);
}

Bool vect_insert(const Vect* const vt,
                 const U64 elem_size,
                 const U8* const elem_bytes,
                 const U64 idx)
{
    // TODO: IMPLEMENT!
    return false;
}

Bool vect_push(const Vect* const vt,
               const U64 elem_size,
               const U8* const elem_bytes)
{
    // TODO: IMPLEMENT!
    return false;
}

void* vect_pop(const Vect* const vt)
{
    // TODO: IMPLEMENT!
}
