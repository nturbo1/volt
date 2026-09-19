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
        del_vecSError(eh->errors);
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

SError* new_error(EErrorType type, SToken tok)
{
    SError* err = (SError*) malloc(sizeof(SError));
    ASSERT(err != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SError obj");
    *( (SToken*) &(err->tok) ) = tok;
    *( (EErrorType*) &(err->type) ) = type;

    return err;
}

void del_error(SError* err)
{
    free(err);
}

// ===========================================================================
// =============================== Vec<SError> ===============================
// ===========================================================================
Vec* new_vecSError(const U64 len, const U64 cap)
{
    return new_vec(len, cap, sizeof(SError));
}

void del_vecSError(Vec* vecSError)
{
    del_vec(vecSError);
}

void vecSError_push(Vec* v, const SError* const err)
{
    vec_push(v, sizeof(SError), (U8*) err);
}
