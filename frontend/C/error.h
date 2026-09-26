#ifndef FRONTEND_C_ERROR_H
#define FRONTEND_C_ERROR_H

#include "token.h"
#include "base_inc.h"

#include <stdlib.h>
#include <stdbool.h>

typedef enum EErrorType
{
    EErrorTypeBeg,
    EERROR_TYPE_NO_ERROR,
    EERROR_TYPE_INVALID_IDENT,
    EERROR_TYPE_INVALID_DECIMAL_LIT,
    EERROR_TYPE_INVALID_HEX_LIT,
    EERROR_TYPE_INVALID_OCTAL_LIT,
    EERROR_TYPE_INVALID_BINARY_LIT,
    EErrorTypeEnd
}
EErrorType;

const String* getErrMsg(EErrorType err);

typedef struct SError
{
    const SToken tok;
    const EErrorType type;
}
SError;

SError* new_error(EErrorType type, SToken tok);
void del_error(SError* err);

// ===========================================================================
// =============================== SErrHandler ===============================
// ===========================================================================
typedef struct SErrHandler
{
    Vec* errors;
    const U64 errLimit; // Max # of errors to be tolerated during compilation
}
SErrHandler;

SErrHandler* new_errHandler();
void del_errHandler(SErrHandler* eh);
void eh_handle(SErrHandler* eh, SError* err);
bool eh_isLimitHit(SErrHandler* eh);

// ===========================================================================
// =============================== Vec<SError> ===============================
// ===========================================================================
Vec* new_vecSError(const U64 len, const U64 cap);
void del_vecSError(Vec* vecSError);
void vecSError_push(Vec* v, const SError* const err);

#endif // FRONTEND_C_ERROR_H
