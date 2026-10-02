#include "base_inc.h"
#include "frontend/C/scanner.h"
#include "frontend/C/parser.h"

#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("No file is provided!\n");
        return 0;
    }

    String* filepath = new_stringFromLit(argv[1]);
    ASSERT(filepath != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "String");
    SParser* parser = new_parser(filepath);
    ASSERT(parser, "Failed to create Parser.");

    // If there is a lexer/scanner error already detected,
    // then we don't build an AST, keep scanning for more
    // errors until we hit the error count limit, print
    // the errors, and stop the compilation.
    if (parser->eh->errs->len > 0)
    {
        while(!eh_isLimitHit(parser->eh))
            parserNextToken(parser);
        // TODO: the output filepath to which the errors are
        //       printed out should be specified somewhere in
        //       the configs or passed as a parameter or sth.
        eh_printErrs(parser->eh, NULL, filepath);
        goto defer;
    }

    parserBuildAST(parser);

defer:
    del_string(filepath);
    del_parser(parser);

    return 0;
}
