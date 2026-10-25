#include "diagnostics.h"
#include "config.h"
#include "vlt_assert.h"
#include "vlt_vector.h"

#include <stdio.h>

#define DEFAULT_MAX_ERROR_C0UNT 20

void diagnosticHandler_init(SDiagnosticHandler *const diagnosticHandler,
                            FILE *const out,
                            const I64 maxErrCount)
{
    ASSERT(diagnosticHandler, NULL_POINTER_ERROR_MSG_FORMAT, "SDiagnosticHandler");
    *(FILE **)&(diagnosticHandler->out) = out ? out : stderr;
    *(I64 *)&(diagnosticHandler->maxErrCount)
        = maxErrCount > 0 ? maxErrCount : DEFAULT_MAX_ERROR_C0UNT;
    if (diagnosticHandler->errors) {
        del_vec(diagnosticHandler);
        *(Vec **)&(diagnosticHandler->errors) = NULL;
    }
    *(Vec **)&(diagnosticHandler->errors) = new_vecSDiagnosticInfo(0, 0, NULL, NULL, NULL);

    if (diagnosticHandler->warnings) {
        del_vec(diagnosticHandler->warnings);
        *(Vec **)&(diagnosticHandler->warnings) = NULL;
    }
    *(Vec **)&(diagnosticHandler->warnings) = new_vecSDiagnosticInfo(0, 0, NULL, NULL, NULL);
}

Bool diagnosticHandler_handle(SDiagnosticHandler *const diagnosticHandler,
                              const EDiagnostic diagnostic,
                              const STokenInfo *const tokenInfo)
{
    ASSERT(diagnosticHandler, NULL_POINTER_ERROR_MSG_FORMAT, "SDiagnosticHandler");
    ASSERT(tokenInfo, NULL_POINTER_ERROR_MSG_FORMAT, "STokenInfo");
    ASSERT_DBG(EDiagnosticBeg < diagnostic && diagnostic < EDiagnosticEnd,
               S_ENUM_VALUE_D_IS_OUT_OF_RANGE_FORMAT,
               "EDiagnostic",
               diagnostic);

    if (eDiagnostic_isFatal() || diagnosticHandler_errCountMaxLimReached(diagnosticHandler))
        return false;

    const SDiagnosticInfo diagnosticInfo{ .diagnostic = diagnostic, .tokenInfo = tokenInfo };
    if (eDiagnostic_isError())
        vecSDiagnosticInfo_push(diagnosticHandler->errors, &diagnosticInfo);
    else if (eDiagnostic_isWarning())
        vecSDiagnosticInfo_push(diagnosticHandler->warnings, &diagnosticInfo);
    else
        ASSERT_DBG(false, "You shouldn't have conditionally branched into here!");
}

void diagnosticHandler_print(const SDiagnosticHandler *const diagnosticHandler)
{
    ASSERT(diagnosticHandler, NULL_POINTER_ERROR_MSG_FORMAT, "SDiagnosticHandler");
    ASSERT_DBG(false, "FIXME: Implement diagnostics printer!");
}
