#include "error.h"
#include "base_inc.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

SErrHandler* new_errHandler(const U64 errCountMax)
{
    SErrHandler* eh = (SErrHandler*) malloc(sizeof(SErrHandler));
    ASSERT(eh != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SErrHandler obj");
    *( (U64*)&(eh->errLimit) ) = errCountMax;
    eh->errs = new_vecSError(0, eh->errLimit);

    return eh;
}

void del_errHandler(SErrHandler* eh)
{
    if (eh != NULL)
    {
        del_vec(eh->errs);
        free(eh);
    }
}

bool eh_handle(SErrHandler* const eh, const SError* const err)
{
    if (eh->errs->len < eh->errLimit)
        vecSError_push(eh->errs, err);

    return eh_isLimitHit(eh);
}

bool eh_isLimitHit(const SErrHandler* const eh)
{
    if (eh->errs->len >= eh->errLimit)
        return true;

    return false;
}

static const String errMsgs[EErrorTypeEnd + 1];

const String* getErrMsg(const EErrorType err)
{
    return &errMsgs[err];
}

#define ERROR_MSG_OUTPUT_FMT "%s:%zu:%zu: error: %s"
void eh_printErrs(const SErrHandler* const eh,
                  const String* const outFilepath,
                  const String* const srcFilepath)
{
    ASSERT(eh, NULL_POINTER_ERROR_MSG_FORMAT, "SErrHandler");
    ASSERT(srcFilepath, "NULL pointer to the source filepath String was passed.");
    ASSERT_DBG(outFilepath != NULL,
               "NULL pointer to the output filepath String was passed.");
    FILE* outFile = NULL;
    if (outFilepath != NULL)
        outFile = fopen((const char*) outFilepath->bytes, "w");
    else
        outFile = stdout;

    for (U64 i = 0; i < eh->errs->len; i++)
    {
        SError* err = vecSError_get(eh->errs, i);
        const String* msg = getErrMsg(err->type);
        fprintf(outFile,
                ERROR_MSG_OUTPUT_FMT,
                (const char*) srcFilepath->bytes,
                err->tok.base.ln,
                err->tok.base.col,
                (const char*) msg->bytes);
        fprintf(outFile, "\n");
    }

    fflush(outFile);
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
