#ifndef FRONTEND_C_COMPILER_H
#define FRONTEND_C_COMPILER_H

#include "diagnostics.h"

#include <stdbool.h>

typedef struct SCompiler {
    SDiagnostics *diags;
    Bool bAbort;
} SCompiler;

void initSCompiler(SCompiler *const comp, SDiagnostics *const diags);
void del_sCompiler(SCompiler *comp);

#endif // FRONTEND_C_COMPILER_H
