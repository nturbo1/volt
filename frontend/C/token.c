#include "token.h"
#include <base_inc.h>

#include <stdlib.h>

SToken* new_tok(const U64 col, const U64 ln, const EToken type)
{
    SToken* tok = (SToken*) malloc(sizeof(SToken));
    ASSERT(tok != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SToken obj");
    tok->col = col;
    tok->ln = ln;
    tok->type = type;

    return tok;
}

STokenIdent* new_tokIdent(const U64 col, const U64 ln, const EToken type, String* const lexeme)
{
    STokenIdent* ident = (STokenIdent*) malloc(sizeof(STokenIdent));
    ASSERT(ident != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "STokenIdent obj");
    ident->tok.col = col;
    ident->tok.ln = ln;
    ident->tok.type = type;
    ident->lexeme = lexeme;

    return ident;
}

STokenNum* new_tokNum(const U64 col, const U64 ln, const EToken type, const U64 val)
{
    STokenNum* number = (STokenNum*) malloc(sizeof(STokenNum));
    ASSERT(number != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "STokenNum obj");
    number->tok.col = col;
    number->tok.ln = ln;
    number->tok.type = type;
    number->val = val;

    return number;
}

void del_tok(SToken* tok)
{
    free(tok);
}

void del_tokIdent(STokenIdent* tok)
{
    if (tok != NULL)
    {
        del_string(tok->lexeme);
        free(tok);
    }
}

void del_tokNum(STokenNum* tok)
{
    free(tok);
}
