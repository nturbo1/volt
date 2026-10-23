#ifndef FRONTEND_C_DIAGNOSTICS_H
#define FRONTEND_C_DIAGNOSTICS_H

#include "token.h"
#include "base_inc.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

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

typedef enum EErrorLevel
{
    EERRORLEVEL_WARNING,
    EERRORLEVEL_ERROR,
    EERRORLEVEL_FATAL
}
EErrorLevel;

const String* getErrMsg(const EErrorType err);

typedef struct SError
{
    const UToken tok;
    const EErrorType type;
    const EErrorLevel lvl;
}
SError;

SError* new_error(const EErrorType type, const UToken* const tok, const EErrorLevel lvl);
void initError(SError* err, const EErrorType type, const UToken* const tok);
void del_error(SError* err);


// ===========================================================================
// =============================== SErrHandler ===============================
// ===========================================================================
GEN_VECTOR_OF(SError)
typedef struct SErrHandler
{
    Vec* errs; // Vec<SError>
    const U64 errLimit; // Max # of errors to be tolerated during compilation
}
SErrHandler;

SErrHandler* new_sErrHandler(const U64 errLimit);
void del_sErrHandler(SErrHandler* eh);

/*
 * @return True if the error has been handled successfully or
 *         False if the error count limit has been reached
 */
Bool eh_handle(SErrHandler* const eh, const SError* const err);

/*
 * @return True if the error count limit has been reached and
 *         False otherwise.
 */
Bool eh_isLimitHit(const SErrHandler* const eh);

void eh_printErrs(const SErrHandler* const eh,
                  const String* const outFilepath,
                  const String* const srcFilepath);


// ===========================================================================
// =============================== SDiagnostics ==============================
// ===========================================================================
typedef struct SDiagnostics
{
    SErrHandler* const eh;

    FILE* out;
}
SDiagnostics;

SDiagnostics* new_sDiagnostics(const U64 errCountMax, FILE* const out);
void del_sDiagnostics(SDiagnostics* diags);

inline Bool diags_reportErr(SDiagnostics* const diags, const SError* const err)
{
    ASSERT(diags, NULL_POINTER_ERROR_MSG_FORMAT, "SDiagnostics");
    return eh_handle(diags->eh, err);
}

#endif // FRONTEND_C_DIAGNOSTICS_H
