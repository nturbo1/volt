#include "error.h"
#include "base_inc.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_EH_ERR_LIMIT 20
SErrHandler* new_errHandler()
{
    SErrHandler* eh = (SErrHandler*) malloc(sizeof(SErrHandler));
    ASSERT(eh != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SErrHandler obj");
    *( (U64*)&(eh->errLimit) ) = DEFAULT_EH_ERR_LIMIT;
    eh->errors = new_vecSError(0, eh->errLimit);

    return eh;
}

void del_errHandler(SErrHandler* eh)
{
    if (eh != NULL)
    {
        del_vec(eh->errors);
        free(eh);
    }
}

void eh_handle(SErrHandler* eh, SError* err)
{
    if (eh->errors->len < eh->errLimit)
        vecSError_push(eh->errors, err);
}

bool eh_isLimitHit(SErrHandler* eh)
{
    if (eh->errors->len >= eh->errLimit)
        return true;

    return false;
}

static const String errMsgs[EErrorTypeEnd + 1];

const String* getErrMsg(EErrorType err)
{
    return &errMsgs[err];
}

static const String errMsgs[EErrorTypeEnd + 1] = {
    [EErrorTypeBeg] = { .len = 0, .bytes = (U8*) "" },
    [EERROR_TYPE_INVALID_IDENT] = { .len = 22, .bytes = (U8*) "Invalid identifier %s." },
    [EErrorTypeEnd] = { .len = 0, .bytes = (U8*) "" }
};

// =====================================================================
// ============================== SError ===============================
// =====================================================================
SError* new_error(const EErrorType type, const UToken* const tok)
{
    SError* err = (SError*) malloc(sizeof(SError));
    ASSERT(err != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "UError obj");
    *( (UToken*) &(err->tok) ) = *tok;
    *( (EErrorType*) &(err->type) ) = type;

    return err;
}

void initError(SError* const err, const EErrorType type, const UToken* const tok)
{
    ASSERT(err != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "UError");
    *( (UToken*) &(err->tok) ) = *tok;
    *( (EErrorType*) &(err->type) ) = type;
}

void del_error(SError* err)
{
    free(err);
}

// =====================================================================
// ============================ Vec<SError> ============================
// =====================================================================
Vec* new_vecSError(const U64 len, const U64 cap)
{
    return new_vec(len, cap, sizeof(SError));
}

void vecSError_insert(Vec* const vt, const U64 idx, const SError* const err)
{
    vec_insert(vt, idx, (const U8* const) err, sizeof(SError));
}

void vecSError_push(Vec* const vt, const SError* const err)
{
    vec_push(vt, sizeof(SError), (const U8* const) err);
}

SError* vecSError_pop(Vec* const vt)
{
    return (SError*) vec_pop(vt);
}

SError* vecSError_get(Vec* const vt, const U64 idx)
{
    return (SError*) vec_get(vt, idx);
}
