#include "token.h"
#include "config.h"

#include <stdlib.h>

Bool eTokenIsKeyword(const EToken tok)
{
    return eTokenKeywordBeg < tok && tok < eTokenKeywordEnd;
}

Bool eTokenIsStorageClass(const EToken tok)
{
    switch (tok) {
    case ETOKEN_STATIC:
    case ETOKEN_EXTERN:
    case ETOKEN_TYPEDEF:
    case ETOKEN_AUTO:
    case ETOKEN_REGISTER:
        return true;

    default:
        return false;
    }
}

Bool eTokenIsTypeSpecifier(const EToken tok)
{
    ASSERT_DBG(eTokenBeg < tok && tok < eTokenEnd,
               "Out of range EToken value was passed: %d.",
               tok);

    switch (tok) {
    case ETOKEN_STRUCT:
    case ETOKEN_UNION:
    case ETOKEN_ENUM:
    case ETOKEN_CONST:
    case ETOKEN_VOID:
    case ETOKEN_SIGNED:
    case ETOKEN_UNSIGNED:
    case ETOKEN_CHAR:
    case ETOKEN_SHORT:
    case ETOKEN_INT:
    case ETOKEN_LONG:
    case ETOKEN_FLOAT:
    case ETOKEN_DOUBLE:
    case ETOKEN__BOOL:
    case ETOKEN__COMPLEX:
        return true;
    default:
        return false;
    }
}

Bool eTokenIsTypeQualifier(const EToken tok)
{
    switch (tok) {
    case ETOKEN_CONST:
    case ETOKEN_RESTRICT:
    case ETOKEN_VOLATILE:
        return true;

    default:
        return false;
    }
}

Bool eTokenIsFuncSpecifier(const EToken tok)
{
    return tok == ETOKEN_INLINE;
}

static const char *eTokenNames[eTokenEnd - eTokenBeg + 1]
    = { [eTokenBeg] = "eTokenBeg",
        [ETOKEN_NO_VALUE] = "ETOKEN_NO_VALUE",
        [ETOKEN_INVALID] = "ETOKEN_INVALID",
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
        [eTokenEnd] = "eTokenEnd" };

const char *eTokenToCStr(EToken tok)
{
    ASSERT(eTokenBeg <= tok && tok <= eTokenEnd, "tok enum value is out of range.");
    return eTokenNames[tok];
}
