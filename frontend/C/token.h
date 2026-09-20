#ifndef FRONTEND_C_TOKEN_H
#define FRONTEND_C_TOKEN_H

#include "base_inc.h"

typedef enum EToken
{
    eTokenBeg,

    ETOKEN_NO_VALUE, // meaning no token has been scanned or just absence of token
    ETOKEN_INVALID_IDENT,
	ETOKEN_EOF,
	ETOKEN_COMMENT,

    ETOKEN_PREPROCESSOR_DIRECTIVE,

    eTokenPunctuatorBeg,

	ETOKEN_LPAREN, // (
	ETOKEN_LBRACK, // [
	ETOKEN_LBRACE, // {

	ETOKEN_RPAREN,    // )
	ETOKEN_RBRACK,    // ]
	ETOKEN_RBRACE,    // }

	ETOKEN_COMMA,  // ,
	ETOKEN_PERIOD, // .
	ETOKEN_COLON,     // :
	ETOKEN_SEMICOLON, // ;
	ETOKEN_ELLIPSIS, // ...
    ETOKEN_ASTERISK, // *

    eTokenPunctuatorEnd,

	eTokenLiteralBeg,
	// Identifiers and basic type literals
	// (these tokens stand for classes of literals)
	ETOKEN_IDENT,  // main
	ETOKEN_INT_LIT,    // 12345
	ETOKEN_FLOAT_LIT,  // 123.45
	ETOKEN_CHAR_LIT,   // 'a'
	ETOKEN_STRING_LIT, // "abc"

	eTokenLiteralEnd,

	eTokenOperatorBeg,
	// Operators and delimiters
	ETOKEN_ADD, // +
	ETOKEN_SUB, // -
	ETOKEN_MUL, // *
	ETOKEN_QUO, // /
	ETOKEN_REM, // %

	ETOKEN_AND,     // &
	ETOKEN_OR,      // |
	ETOKEN_XOR,     // ^
	ETOKEN_SHL,     // <<
	ETOKEN_SHR,     // >>
	ETOKEN_AND_NOT, // &^

	ETOKEN_ADD_ASSIGN, // +=
	ETOKEN_SUB_ASSIGN, // -=
	ETOKEN_MUL_ASSIGN, // *=
	ETOKEN_QUO_ASSIGN, // /=
	ETOKEN_REM_ASSIGN, // %=

	ETOKEN_AND_ASSIGN,     // &=
	ETOKEN_OR_ASSIGN,      // |=
	ETOKEN_XOR_ASSIGN,     // ^=
	ETOKEN_SHL_ASSIGN,     // <<=
	ETOKEN_SHR_ASSIGN,     // >>=
	ETOKEN_AND_NOT_ASSIGN, // &^=

	ETOKEN_LAND,  // &&
	ETOKEN_LOR,   // ||
	ETOKEN_RARROW, // ->
	ETOKEN_INC,   // ++
	ETOKEN_DEC,   // --

	ETOKEN_EQL,    // ==
	ETOKEN_LSS,    // <
	ETOKEN_GTR,    // >
	ETOKEN_ASSIGN, // =
	ETOKEN_NOT,    // !

	ETOKEN_NEQ,      // !=
	ETOKEN_LEQ,      // <=
	ETOKEN_GEQ,      // >=

	eTokenOperatorEnd,

	eTokenKeywordBeg,
	// Keywords
	ETOKEN_CONST,
	ETOKEN_IF,
	ETOKEN_ELSE,
	ETOKEN_FOR,
	ETOKEN_BREAK,
	ETOKEN_CONTINUE,
	ETOKEN_GOTO,
	ETOKEN_RETURN,
	ETOKEN_STRUCT,
	ETOKEN_UNION,
	ETOKEN_ENUM,
	ETOKEN_SWITCH,
	ETOKEN_CASE,
	ETOKEN_DEFAULT,
	ETOKEN_WHILE,
	ETOKEN_DO,
	ETOKEN_VOID,
	ETOKEN_STATIC,
	ETOKEN_EXTERN,
	ETOKEN_REGISTER,
	ETOKEN_SIGNED,
	ETOKEN_UNSIGNED,
	ETOKEN_SIZEOF,
	ETOKEN_TYPEDEF,
	ETOKEN_VOLATILE,
    ETOKEN_AUTO,
    ETOKEN_INLINE,          // (C99)
    ETOKEN_RESTRICT,        // (C99)
    ETOKEN__COMPLEX,         // (C99)
    ETOKEN__IMAGINARY,       // (C99)
    ETOKEN__BOOL,            // (C99)(deprecated in C23)
    // ETOKEN__ATOMIC,          // (C11)
    // ETOKEN__GENERIC,         // (C11)

	ETOKEN_CHAR,
	ETOKEN_SHORT,
	ETOKEN_INT,
	ETOKEN_LONG,
	ETOKEN_FLOAT,
	ETOKEN_DOUBLE,

	eTokenKeywordEnd,

    // TODO: Consider the below tokens later!
    //
    // [ETOKEN_] = "_Alignas",             // (C11)(deprecated in C23)
    // [ETOKEN_] = "_Alignof",             // (C11)(deprecated in C23)
    // [ETOKEN_] = "_Noreturn",            // (C11)(deprecated in C23)
    // [ETOKEN_] = "_Static_assert",       // (C11)(deprecated in C23)
    // [ETOKEN_] = "_Thread_local",        // (C11)(deprecated in C23)
    //
    // [ETOKEN_] = "_BitInt",              // (C23)
    // [ETOKEN_] = "_Decimal128",          // (C23)
    // [ETOKEN_] = "_Decimal32",           // (C23)
    // [ETOKEN_] = "_Decimal64",           // (C23)
    // [ETOKEN_] = "alignas"               // (C23)
    // [ETOKEN_] = "alignof",              // (C23)
    // [ETOKEN_] = "bool",                 // (C23)
    // [ETOKEN_] = "constexpr",            // (C23)
    // [ETOKEN_] = "typeof",               // (C23)
    // [ETOKEN_] = "typeof_unqual",        // (C23)
    // [ETOKEN_] = "thread_local",         // (C23)
    // [ETOKEN_] = "true",                 // (C23)
    // [ETOKEN_] = "static_assert",        // (C23)
    // [ETOKEN_] = "false",                // (C23)
    // [ETOKEN_] = "nullptr",              // (C23)

    eTokenEnd
}
EToken;

const char* eTokenToCStr(EToken tok);

typedef struct SToken
{
    U64 col;
    U64 ln;
    EToken type; // also the indicator of the underlying data type
                 // that inherits from SToken
}
SToken;

typedef struct STokenIdent
{
    SToken tok;
    String* lexeme;
}
STokenIdent;

typedef struct STokenNum
{
    SToken tok;
    U64 val;
}
STokenNum;

typedef union UToken
{
    SToken base; // base.type is the indicator of the underlying data type
    STokenIdent ident;
    STokenNum number;
}
UToken;

SToken* new_tok(const U64 col, const U64 ln, const EToken type);
STokenIdent* new_tokIdent(const U64 col, const U64 ln, const EToken type, String* const lexeme);
STokenNum* new_tokNum(const U64 col, const U64 ln, const EToken type, const U64 val);
void del_tok(SToken* tok);
void del_tokIdent(STokenIdent* tok);
void del_tokNum(STokenNum* tok);

#endif // FRONTEND_C_TOKEN_H
