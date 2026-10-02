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

const String* getErrMsg(const EErrorType err);

typedef struct SError
{
    const UToken tok;
    const EErrorType type;
}
SError;

SError* new_error(const EErrorType type, const UToken* const tok);
void initError(SError* err, const EErrorType type, const UToken* const tok);
void del_error(SError* err);


// ===========================================================================
// =============================== SErrHandler ===============================
// ===========================================================================
typedef struct SErrHandler
{
    Vec* errs;
    const U64 errLimit; // Max # of errors to be tolerated during compilation
}
SErrHandler;

SErrHandler* new_errHandler();
void del_errHandler(SErrHandler* eh);

/*
 * @return True if the error has been handled successfully or
 *         False if the error count limit has been reached
 */
bool eh_handle(SErrHandler* const eh, const SError* const err);

/*
 * @return True if the error count limit has been reached and
 *         False otherwise.
 */
bool eh_isLimitHit(const SErrHandler* const eh);

void eh_printErrs(const SErrHandler* const eh,
                  const String* const outFilepath,
                  const String* const srcFilepath);

// =====================================================================
// ============================ Vec<SError> ============================
// =====================================================================
Vec* new_vecSError(const U64 len, const U64 cap);
void vecSError_insert(Vec* const vt, const U64 idx, const SError* const err);
void vecSError_push(Vec* const vt, const SError* const err);
SError* vecSError_pop(Vec* const vt);
SError* vecSError_get(Vec* const vt, const U64 idx);

#endif // FRONTEND_C_ERROR_H
