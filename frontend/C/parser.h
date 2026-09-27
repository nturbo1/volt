#ifndef FRONTEND_C_PARSER_H
#define FRONTEND_C_PARSER_H

#include "scanner.h"
#include "ast.h"
#include "token.h"

typedef struct STokenRing STokenRing;
typedef struct SParser
{
    SScanner* scanner;
    SParserAST* ast;
    STokenRing* tokLookAheadRing;
    Vec* errs;
}
SParser;

SParser* new_parser(String* filepath);
void del_parser(SParser* parser);
const UToken* parserNextToken(SParser* parser);
const UToken* parserPeekToken(SParser* parser);

#endif // FRONTEND_C_PARSER_H
