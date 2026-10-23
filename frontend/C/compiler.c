#include "compiler.h"
#include "base_inc.h"

#include <stdlib.h>

void initCompiler(SCompiler* const comp, SDiagnostics* const diags)
{
    ASSERT(comp, NULL_POINTER_ERROR_MSG_FORMAT, "SCompiler");
    if (!diags)
        comp->diags = new_sDiagnostics();
    else
        comp->diags = diags;

    comp->bAbort = false;
}

void del_sCompiler(SCompiler* comp)
{
    if (comp)
    {
        del_sDiagnostics(comp->diags);
        free(comp);
    }
}
