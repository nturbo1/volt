#ifndef FRONTEND_C_DIAGNOSTICS_H
#define FRONTEND_C_DIAGNOSTICS_H

#include "config.h"
#include "token.h"
#include "vlt_assert.h"
#include "vlt_vector.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum EDiagnostic {
    EDiagnosticBeg,
    EDiagnosticWarningBeg,
    EDiagnosticWarningEnd,
    EDiagnosticErrorBeg,
    EDiagnosticErrorEnd,
    EDiagnosticFatalBeg,
    EDiagnosticFatalEnd,
    EDiagnosticEnd
} EDiagnostic;

inline Bool eDiagnostic_isWarning(const EDiagnostic diagnostic)
{
    ASSERT_DBG(EDiagnosticBeg < diagnostic && diagnostic < EDiagnosticEnd,
               S_ENUM_VALUE_D_IS_OUT_OF_RANGE_FORMAT,
               "EDiagnostic",
               diagnostic);
    return (EDiagnosticWarningBeg < diagnostic) && (diagnostic < EDiagnosticWarningEnd);
}

inline Bool eDiagnostic_isError(const EDiagnostic diagnostic)
{
    ASSERT_DBG(EDiagnosticBeg < diagnostic && diagnostic < EDiagnosticEnd,
               S_ENUM_VALUE_D_IS_OUT_OF_RANGE_FORMAT,
               "EDiagnostic",
               diagnostic);
    return (EDiagnosticErrorBeg < diagnostic) && (diagnostic < EDiagnosticErrorEnd);
}

inline Bool eDiagnostic_isFatal(const EDiagnostic diagnostic)
{
    ASSERT_DBG(EDiagnosticBeg < diagnostic && diagnostic < EDiagnosticEnd,
               S_ENUM_VALUE_D_IS_OUT_OF_RANGE_FORMAT,
               "EDiagnostic",
               diagnostic);
    return (EDiagnosticFatalBeg < diagnostic) && (diagnostic < EDiagnosticFatalEnd);
}

typedef struct SDiagnosticInfo {
    const EDiagnostic diagnostic;
    const STokenInfo tokenInfo;
} SDiagnosticInfo;

GEN_VECTOR_OF(SDiagnosticInfo)
typedef struct SDiagnosticHandler {
    FILE *const out;
    const I64 maxErrCount;
    // Vec<SDiagnosticInfo>
    Vec *const errors;
    // Vec<SDiagnosticInfo>
    Vec *const warnings;
} SDiagnosticHandler;

void diagnosticHandler_init(SDiagnosticHandler *const diagnosticHandler,
                            FILE *const out,
                            const I64 maxErrCount);

Bool diagnosticHandler_handle(SDiagnosticHandler *const diagnosticHandler,
                              const EDiagnostic diagnostic,
                              const STokenInfo *const tokenInfo);

void diagnosticHandler_print(const SDiagnosticHandler *const diagnosticHandler);

inline Bool diagnosticHandler_errCountMaxLimReached(
    const SDiagnosticHandler *const diagnosticHandler)
{
    ASSERT_DBG(diagnosticHandler->errors->len <= (U64)I64_MAX,
               "Diagnostic vector size shouldn't have gotten this big: %zu!",
               diagnosticHandler->errors->len);
    return diagnosticHandler->maxErrCount <= (I64)diagnosticHandler->errors->len;
}

#endif // FRONTEND_C_DIAGNOSTICS_H
