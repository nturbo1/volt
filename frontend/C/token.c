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

static const char* eTokenNames[eTokenEnd - eTokenBeg + 1] = {
    [eTokenBeg] = "eTokenBeg",
    [ETOKEN_NO_VALUE] = "ETOKEN_NO_VALUE",
    [ETOKEN_INVALID_IDENT] = "ETOKEN_INVALID_IDENT",
	[ETOKEN_EOF] = "ETOKEN_EOF",
	[ETOKEN_COMMENT] = "ETOKEN_COMMENT",
    [ETOKEN_PREPROCESSOR_DIRECTIVE] = "ETOKEN_PREPROCESSOR_DIRECTIVE",
    [eTokenPunctuatorBeg] = "eTokenPunctuatorBeg",
	[ETOKEN_LPAREN] = "ETOKEN_LPAREN",
	[ETOKEN_LBRACK] = "ETOKEN_LBRACK",
	[ETOKEN_LBRACE] = "ETOKEN_LBRACE",
	[ETOKEN_RPAREN] = "ETOKEN_RPAREN",
	[ETOKEN_RBRACK] = "ETOKEN_RBRACK",
	[ETOKEN_RBRACE] = "ETOKEN_RBRACE",
	[ETOKEN_COMMA] = "ETOKEN_COMMA",
	[ETOKEN_PERIOD] = "ETOKEN_PERIOD",
	[ETOKEN_COLON] = "ETOKEN_COLON",
	[ETOKEN_SEMICOLON] = "ETOKEN_SEMICOLON",
	[ETOKEN_ELLIPSIS] = "ETOKEN_ELLIPSIS",
    [ETOKEN_ASTERISK] = "ETOKEN_ASTERISK",
    [eTokenPunctuatorEnd] = "eTokenPunctuatorEnd",
	[eTokenLiteralBeg] = "eTokenLiteralBeg",
	[ETOKEN_IDENT] = "ETOKEN_IDENT",
	[ETOKEN_INT_LIT] = "ETOKEN_INT_LIT",
	[ETOKEN_FLOAT_LIT] = "ETOKEN_FLOAT_LIT",
	[ETOKEN_CHAR_LIT] = "ETOKEN_CHAR_LIT",
	[ETOKEN_STRING_LIT] = "ETOKEN_STRING_LIT",
	[eTokenLiteralEnd] = "eTokenLiteralEnd",
	[eTokenOperatorBeg] = "eTokenOperatorBeg",
	[ETOKEN_ADD] = "ETOKEN_ADD",
	[ETOKEN_SUB] = "ETOKEN_SUB",
	[ETOKEN_MUL] = "ETOKEN_MUL",
	[ETOKEN_QUO] = "ETOKEN_QUO",
	[ETOKEN_REM] = "ETOKEN_REM",
	[ETOKEN_AND] = "ETOKEN_AND",
	[ETOKEN_OR] = "ETOKEN_OR",
	[ETOKEN_XOR] = "ETOKEN_XOR",
	[ETOKEN_SHL] = "ETOKEN_SHL",
	[ETOKEN_SHR] = "ETOKEN_SHR",
	[ETOKEN_AND_NOT] = "ETOKEN_AND_NOT",
	[ETOKEN_ADD_ASSIGN] = "ETOKEN_ADD_ASSIGN",
	[ETOKEN_SUB_ASSIGN] = "ETOKEN_SUB_ASSIGN",
	[ETOKEN_MUL_ASSIGN] = "ETOKEN_MUL_ASSIGN",
	[ETOKEN_QUO_ASSIGN] = "ETOKEN_QUO_ASSIGN",
	[ETOKEN_REM_ASSIGN] = "ETOKEN_REM_ASSIGN",
	[ETOKEN_AND_ASSIGN] = "ETOKEN_AND_ASSIGN",
	[ETOKEN_OR_ASSIGN] = "ETOKEN_OR_ASSIGN",
	[ETOKEN_XOR_ASSIGN] = "ETOKEN_XOR_ASSIGN",
	[ETOKEN_SHL_ASSIGN] = "ETOKEN_SHL_ASSIGN",
	[ETOKEN_SHR_ASSIGN] = "ETOKEN_SHR_ASSIGN",
	[ETOKEN_AND_NOT_ASSIGN] = "ETOKEN_AND_NOT_ASSIGN",
	[ETOKEN_LAND] = "ETOKEN_LAND",
	[ETOKEN_LOR] = "ETOKEN_LOR",
	[ETOKEN_RARROW] = "ETOKEN_RARROW",
	[ETOKEN_INC] = "ETOKEN_INC",
	[ETOKEN_DEC] = "ETOKEN_DEC",
	[ETOKEN_EQL] = "ETOKEN_EQL",
	[ETOKEN_LSS] = "ETOKEN_LSS",
	[ETOKEN_GTR] = "ETOKEN_GTR",
	[ETOKEN_ASSIGN] = "ETOKEN_ASSIGN",
	[ETOKEN_NOT] = "ETOKEN_NOT",
	[ETOKEN_NEQ] = "ETOKEN_NEQ",
	[ETOKEN_LEQ] = "ETOKEN_LEQ",
	[ETOKEN_GEQ] = "ETOKEN_GEQ",
	[eTokenOperatorEnd] = "eTokenOperatorEnd",
	[eTokenKeywordBeg] = "eTokenKeywordBeg",
	[ETOKEN_CONST] = "ETOKEN_CONST",
	[ETOKEN_IF] = "ETOKEN_IF",
	[ETOKEN_ELSE] = "ETOKEN_ELSE",
	[ETOKEN_FOR] = "ETOKEN_FOR",
	[ETOKEN_BREAK] = "ETOKEN_BREAK",
	[ETOKEN_CONTINUE] = "ETOKEN_CONTINUE",
	[ETOKEN_GOTO] = "ETOKEN_GOTO",
	[ETOKEN_RETURN] = "ETOKEN_RETURN",
	[ETOKEN_STRUCT] = "ETOKEN_STRUCT",
	[ETOKEN_UNION] = "ETOKEN_UNION",
	[ETOKEN_ENUM] = "ETOKEN_ENUM",
	[ETOKEN_SWITCH] = "ETOKEN_SWITCH",
	[ETOKEN_CASE] = "ETOKEN_CASE",
	[ETOKEN_DEFAULT] = "ETOKEN_DEFAULT",
	[ETOKEN_WHILE] = "ETOKEN_WHILE",
	[ETOKEN_DO] = "ETOKEN_DO",
	[ETOKEN_VOID] = "ETOKEN_VOID",
	[ETOKEN_STATIC] = "ETOKEN_STATIC",
	[ETOKEN_EXTERN] = "ETOKEN_EXTERN",
	[ETOKEN_REGISTER] = "ETOKEN_REGISTER",
	[ETOKEN_SIGNED] = "ETOKEN_SIGNED",
	[ETOKEN_UNSIGNED] = "ETOKEN_UNSIGNED",
	[ETOKEN_SIZEOF] = "ETOKEN_SIZEOF",
	[ETOKEN_TYPEDEF] = "ETOKEN_TYPEDEF",
	[ETOKEN_VOLATILE] = "ETOKEN_VOLATILE",
    [ETOKEN_AUTO] = "ETOKEN_AUTO",
    [ETOKEN_INLINE] = "ETOKEN_INLINE",
    [ETOKEN_RESTRICT] = "ETOKEN_RESTRICT",
    [ETOKEN__COMPLEX] = "ETOKEN__COMPLEX",
    [ETOKEN__IMAGINARY] = "ETOKEN__IMAGINARY",
    [ETOKEN__BOOL] = "ETOKEN__BOOL",
  	[ETOKEN_CHAR] = "ETOKEN_CHAR",
	[ETOKEN_SHORT] = "ETOKEN_SHORT",
	[ETOKEN_INT] = "ETOKEN_INT",
	[ETOKEN_LONG] = "ETOKEN_LONG",
	[ETOKEN_FLOAT] = "ETOKEN_FLOAT",
	[ETOKEN_DOUBLE] = "ETOKEN_DOUBLE",
	[eTokenKeywordEnd] = "eTokenKeywordEnd",
    [eTokenEnd] = "eTokenEnd"
};

const char* eTokenToCStr(EToken tok)
{
    ASSERT(eTokenBeg <= tok && tok <= eTokenEnd, "tok enum value is out of range.");
    return eTokenNames[tok];
}
