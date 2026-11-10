#ifndef FRONTEND_C_SCANNER_H
#define FRONTEND_C_SCANNER_H

#include "config.h"
#include "diagnostics.h"
#include "token.h"
#include "vlt_hashmap.h"

// ===============================================================================
// ================================== SCharSrc ===================================
// ===============================================================================
typedef struct CharSrc SCharSrc;
SCharSrc *new_charSrc(const String *const filepath);
void del_charSrc(SCharSrc *src);
// Returns the next character/byte in a byte stream and advances the character/byte position in
// the byte stream.
U8 charSrc_next(SCharSrc *const src);
// Returns the next character/byte in a byte stream and does NOT advance the character/byte
// position in the byte stream.
U8 charSrc_peek(SCharSrc *const src);
// Returns a copy of the src filepath string.
String *charSrc_getFilepath(const SCharSrc *const src);

// ===============================================================================
// ================================== SScanner ===================================
// ===============================================================================
typedef struct Scanner {
    SCharSrc *const src;
    U64 lnOffs; // line offset in the src file
    U64 colOffs; // column offset in the src file
    STokenInfo tokInfo; // the latest scanned token info
    EDiagnostic diagnostic; // lexical diagnostic about the latest scanned token
} SScanner;

SScanner *new_scanner(const String *const filepath);
SScanner *new_scannerExtSrc(SCharSrc *const src);
void del_scanner(SScanner *s);

// Returns the next token info from a token stream and advances the token position in the token
// stream.
EDiagnostic scanner_next(SScanner *const s, STokenInfo *const tokenInfo);
// Returns the next token info from a token stream and does NOT advance the token position in the
// token stream.
const STokenInfo *scanner_peek(SScanner *const s);

static inline EDiagnostic scanner_diagnostic(const SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    return s->diagnostic;
}

///////////////////////////////////////////////////////////////////////////////////
// HashMap<String, EToken>
///////////////////////////////////////////////////////////////////////////////////
void printHashMapStringEToken(const HashMap *const hm);
void printKeywordsHashMap(); // for debugging purposes

#endif // FRONTEND_C_SCANNER_H
