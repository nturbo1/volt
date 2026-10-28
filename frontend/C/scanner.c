#include "scanner.h"
#include "config.h"
#include "token.h"
#include "vlt_hashmap.h"
#include "vlt_memory.h"
#include "vlt_string.h"
#include "vlt_stringBuilder.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// HashMap<String, EToken>
GEN_HASHMAP_OF(String, EToken)
static HashMap *keywords = NULL;
static Bool initKeywordsMap();

// ===============================================================================
// ================================== SCharSrc ===================================
// ===============================================================================
#define CHAR_SRC_BUFFER_SIZE 8192
struct CharSrc {
    FILE *file; // src file
    const String *const filepath; // src filepath
    // Contains 2 buffers:  Buffer 0              Buffer 1
    //                     [ | | | ... | | | |   | |   | ... | | |    ]
    //                      0 1 2 ........... n-1 n n+1 ......... 2n-1
    U64 bufEnd; // end of the current buffer being processed
    U64 next; // the next character index in the src buffer
    U8 buf[2 * CHAR_SRC_BUFFER_SIZE]; // the src buffer
};

// Fills the current buffer that the scanner is currently processing
// with the next chunk of bytes from the src file.
static void charSrc_fillBuf(SCharSrc *src)
{
    ASSERT(src != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SCharSrc");
    U8 *buf = src->buf + src->next;
    fflush(stdout);
    U64 n = (U64)fread(buf, sizeof(U8), CHAR_SRC_BUFFER_SIZE, src->file);

    if (feof(src->file)) {
        buf[n] = EOF;
    }
}

SCharSrc *new_charSrc(const String *const filepath)
{
    ASSERT(!string_isEmpty(filepath), "Filepath can't be empty!");
    FILE *srcFile = fopen((const char *)filepath->bytes, "r");
    ASSERT(srcFile != NULL, "Failed to open file %s\n", (const char *)filepath->bytes);
    SCharSrc *src = (SCharSrc *)malloc(sizeof(SCharSrc));
    ASSERT(src != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SCharSrc");
    src->file = srcFile;
    srcFile = NULL;
    src->bufEnd = CHAR_SRC_BUFFER_SIZE;
    src->next = 0;
    charSrc_fillBuf(src);

    return src;
}

void del_charSrc(SCharSrc *src)
{
    if (src) {
        if (src->file)
            fclose(src->file);
        src->file = NULL;
        del_string((String *)(src->filepath));
        free(src);
    }
}

U8 charSrc_next(SCharSrc *const src)
{
    ASSERT(src != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SCharSrc");
    const U8 ch = src->buf[src->next];

    if ((char)ch != EOF) {
        src->next = ((src->next + 1) % (2 * CHAR_SRC_BUFFER_SIZE));

        // if next is the beginning of one of the buffers
        if (src->next % CHAR_SRC_BUFFER_SIZE == 0) {
            charSrc_fillBuf(src);
            src->bufEnd = ((src->next + CHAR_SRC_BUFFER_SIZE) % (2 * CHAR_SRC_BUFFER_SIZE));
        }
    }

    return ch;
}

U8 charSrc_peek(SCharSrc *const src)
{
    ASSERT(src != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SCharSrc");
    return src->buf[src->next];
}

String *charSrc_getFilepath(const SCharSrc *const src)
{
    ASSERT(src, NULL_POINTER_ERROR_MSG_FORMAT, "SCharSrc");
    return string_copy(src->filepath, 0, src->filepath->len);
}

// ===============================================================================
// ================================== SScanner ===================================
// ===============================================================================
struct Scanner {
    SCharSrc *const src;
    U64 lnOffs; // line offset in the src file
    U64 colOffs; // column offset in the src file
    STokenInfo tokInfo; // the latest scanned token info
    EDiagnostic diagnostic; // lexical diagnostic about the latest scanned token
};

static inline void scanner_resetTokInfo(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->tokInfo.ln = s->lnOffs + 1;
    s->tokInfo.col = s->colOffs + 1;
    s->tokInfo.tok = ETOKEN_EOF;
    del_string(s->tokInfo.lexeme);
    s->tokInfo.lexeme = NULL;
}
static inline void scanner_resetDiagnostic(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->diagnostic = EDiagnosticEnd;
}
inline void scanner_setTokCol(SScanner *const s, const U64 col)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->tokInfo.col = col;
}
inline void scanner_setTokLn(SScanner *const s, const U64 ln)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->tokInfo.ln = ln;
}
inline void scanner_setTok(SScanner *const s, const EToken tok)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->tokInfo.tok = tok;
}
inline void scanner_setTokLexeme(SScanner *const s, String *const lexeme)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->tokInfo.lexeme = lexeme;
}
inline void scanner_setDiagnostic(SScanner *const s, const EDiagnostic diagnostic)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->diagnostic = diagnostic;
}

static U8 nextChar(SScanner *s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    U8 ch = charSrc_next(s->src);

    if ((char)ch != EOF) {
        if ((char)ch == '\n') {
            s->lnOffs++;
            s->colOffs = 0;
        } else {
            s->colOffs++;
        }
    }

    return ch;
}
static inline U8 peekChar(SScanner *s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    return charSrc_peek(s->src);
}

static void scanNext(SScanner *const s);
static void scanIdentifier(SScanner *const s);
// static void scanInt(const SScanner* s);
static void scanNumber(SScanner *const s);
static void scanOctal(SScanner *const s);
static void scanHex(SScanner *const s);
static void scanDecimal(SScanner *const s);
static void scanFloat(SScanner *const s);
static void scanComment(SScanner *const s, const Bool multiLine);

static Bool isWhitespace(const U8 ch);
static Bool isAlpha(const U8 ch);
static Bool isAlnum(const U8 ch);
static Bool isDecDigit(const U8 ch);
static Bool isOctDigit(const U8 ch);
static void skipWhitespace(SScanner *s);

SScanner *scanner_new(const String *const filepath)
{
    SCharSrc *src = new_charSrc(filepath);
    ASSERT_DBG(src, FAILED_CREATE_NEW_S_OBJ_FORMAT, "SCharSrc");
    return scanner_newSrc(src);
}

SScanner *scanner_newSrc(SCharSrc *const src)
{
    ASSERT(src, NULL_POINTER_ERROR_MSG_FORMAT, "SCharSrc");
    SScanner *s = (SScanner *)malloc(sizeof(SScanner));
    ASSERT(s, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SScanner");
    *(SCharSrc **)&(s->src) = src;
    s->lnOffs = s->colOffs = 0;
    STokenInfo _temp;
    scanner_next(s, &_temp); // initializes s->tokInfo and s->diagnostic

    ASSERT(initKeywordsMap(), "Failed to initialize the keywords map!");

    return s;
}

const STokenInfo *scanner_peek(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    return &(s->tokInfo);
}

EDiagnostic scanner_next(SScanner *const s, STokenInfo *const tokenInfo)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // NOTE: The next token is expected to have been scanned already or set to EOF!
    tokenInfo_init(tokenInfo, s->tokInfo.col, s->tokInfo.ln, s->tokInfo.tok, s->tokInfo.lexeme);
    EDiagnostic diagnostic = s->diagnostic;
    if (s->tokInfo.tok == ETOKEN_EOF)
        return diagnostic;

    scanNext(s);
    return diagnostic;
}

// Scans the next token and updates s->tokInfo and s->diagnostic accordingly.
// So that the next token is returned or copied into when needed.
static void scanNext(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    scanner_resetDiagnostic(s);
    scanner_resetTokInfo(s);
    skipWhitespace(s);

    U8 ch = charSrc_peek(s->src);
    if ((char)ch == EOF) {
        ASSERT_DBG(
            s->tokInfo.tok == ETOKEN_EOF,
            "Expected the scanner reset token info function to set the token to ETOKEN_EOF!");
        return;
    }

    if (isDecDigit(ch)) {
        scanNumber(s);
    } else {
        switch (ch) {
        case '(':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_LPAREN;
            break;

        case '[':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_LBRACK;
            break;

        case '{':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_LBRACE;
            break;

        case ')':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_RPAREN;
            break;

        case ']':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_RBRACK;
            break;

        case '}':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_RBRACE;
            break;

        case ',':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_COMMA;
            break;

        case '.': {
            nextChar(s);
            ch = peekChar(s);
            if (ch == '.') {
                nextChar(s);
                ch = peekChar(s);
                if (ch == '.') {
                    nextChar(s);
                    s->tokInfo.tok = ETOKEN_ELLIPSIS;
                    break;
                } else {
                    s->tokInfo.tok = ETOKEN_INVALID;
                    s->tokInfo.lexeme = new_stringFromLit("..", NULL, NULL);
                    s->diagnostic = EDIAGNOSTIC_INVALID_IDENT;
                    break;
                }
            }
            s->tokInfo.tok = ETOKEN_PERIOD;
            break;
        }

        case ':':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_COLON;
            break;

        case ';':
            nextChar(s);
            s->tokInfo.tok = ETOKEN_SEMICOLON;
            break;

        case '*':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_MUL_ASSIGN;
                break;

            default:
                s->tokInfo.tok = ETOKEN_ASTERISK;
                break;
            }
            break;

        case '/':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '/':
                nextChar(s);
                scanComment(s, false);
                break;

            case '*':
                nextChar(s);
                scanComment(s, true);
                break;

            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_QUO_ASSIGN;
                break;

            default:
                s->tokInfo.tok = ETOKEN_QUO;
                break;
            }
            break;

        case '+':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '+':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_INC;
                break;

            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_ADD_ASSIGN;
                break;

            default:
                s->tokInfo.tok = ETOKEN_ADD;
                break;
            }
            break;

        case '-':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '-':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_DEC;
                break;

            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_SUB_ASSIGN;
                break;

            case '>':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_RARROW;
                break;

            default:
                s->tokInfo.tok = ETOKEN_SUB;
                break;
            }
            break;

        case '%':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_REM_ASSIGN;
                break;

            default:
                s->tokInfo.tok = ETOKEN_REM;
                break;
            }
            break;

        case '&':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '^':
                nextChar(s);
                ch = peekChar(s);
                switch (ch) {
                case '=':
                    nextChar(s);
                    s->tokInfo.tok = ETOKEN_AND_NOT_ASSIGN;
                    break;

                default:
                    s->tokInfo.tok = ETOKEN_AND_NOT;
                    break;
                }
                break;

            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_AND_ASSIGN;
                break;

            case '&':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_LAND;
                break;

            default:
                s->tokInfo.tok = ETOKEN_AND;
                break;
            }
            break;

        case '|':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '|':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_LOR;
                break;

            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_OR_ASSIGN;
                break;

            default:
                s->tokInfo.tok = ETOKEN_OR;
                break;
            }
            break;

        case '^':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_XOR_ASSIGN;
                break;

            default:
                s->tokInfo.tok = ETOKEN_XOR;
                break;
            }
            break;

        case '<':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_LEQ;
                break;

            case '<':
                nextChar(s);
                ch = peekChar(s);
                switch (ch) {
                case '=':
                    nextChar(s);
                    s->tokInfo.tok = ETOKEN_SHL_ASSIGN;
                    break;

                default:
                    s->tokInfo.tok = ETOKEN_SHL;
                    break;
                }
                break;

            default:
                s->tokInfo.tok = ETOKEN_LSS;
                break;
            }
            break;

        case '>':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_GEQ;
                break;

            case '>':
                nextChar(s);
                ch = peekChar(s);
                switch (ch) {
                case '=':
                    nextChar(s);
                    s->tokInfo.tok = ETOKEN_SHR_ASSIGN;
                    break;

                default:
                    s->tokInfo.tok = ETOKEN_SHR;
                    break;
                }
                break;

            default:
                s->tokInfo.tok = ETOKEN_GTR;
                break;
            }
            break;

        case '=':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_EQL;
                break;

            default:
                s->tokInfo.tok = ETOKEN_ASSIGN;
                break;
            }
            break;

        case '!':
            nextChar(s);
            ch = peekChar(s);
            switch (ch) {
            case '=':
                nextChar(s);
                s->tokInfo.tok = ETOKEN_NEQ;
                break;

            default:
                s->tokInfo.tok = ETOKEN_NOT;
                break;
            }
            break;

        default:
            scanIdentifier(s);
        }
    }
}

void del_scanner(SScanner *s)
{
    if (s) {
        del_charSrc(s->src);
        del_hashmap(keywords);
        keywords = NULL;
        free(s);
    }
}

static void scanIdentifier(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    scanner_resetTokInfo(s);
    scanner_resetDiagnostic(s);

    U8 ch = peekChar(s);
    if (!isAlpha(ch) && ch != '_') {
        s->tokInfo.tok = ETOKEN_INVALID;
        s->diagnostic = EDIAGNOSTIC_INVALID_IDENT;
    }

    StringBuilder *identSb = new_stringBuilder(NULL, NULL);
    while (isAlnum(ch) || ch == '_') {
        sb_appendChar(identSb, nextChar(s));
        ch = peekChar(s);
    }

    String *ident = sb_toString(identSb);
    EToken tok = *hashmapStringEToken_get(keywords, ident);
    if (eTokenIsKeyword(tok)) {
        s->tokInfo.tok = tok;
        goto cleanupAll;
    }

    s->tokInfo.tok = ETOKEN_IDENT;
    s->tokInfo.lexeme = ident;
    goto cleanupAllButIdent;

cleanupAll:
    del_string(ident);
cleanupAllButIdent:
    del_stringBuilder(identSb);
}

typedef enum ENumBase {
    eNumBaseBeg,
    ENUMBASE_10,
    ENUMBASE_8,
    ENUMBASE_16,
    ENUMBASE_2,
    eNumBaseEnd
} ENumBase;

// TODO: SHOULD PROBABLY BE MOVED TO PARSER!
//
// Converts a given digit character in a given number base
// to its integral value.
// For instance:
//     decimal:     '9' -> 9, '0' -> 0, '3' -> 3, ...
//     hexadecimal: '2' -> 2, '0' -> 0, 'a' -> 10, 'F' -> 15, ...
//     octal:       '4' -> 4, '7' -> 7, '0' -> 0, ...
//     binary:      '0' -> 0, '1' -> 1
// inline static U64 digitFromCharToInt(const U8 digitChar, const ENumBase base, EErrorType *const
// err)
// {
//     *err = EERROR_TYPE_NO_ERROR;
//     ASSERT(eNumBaseBeg < base && base < eNumBaseEnd, "Number base enum value is out of range.");
//
//     switch (base) {
//     case ENUMBASE_10:
//         switch (digitChar) {
//         case '0':
//         case '1':
//         case '2':
//         case '3':
//         case '4':
//         case '5':
//         case '6':
//         case '7':
//         case '8':
//         case '9':
//             return digitChar - '0';
//
//         default:
//             *err = EERROR_TYPE_INVALID_DECIMAL_LIT;
//             return 0;
//         }
//
//     case ENUMBASE_8:
//         switch (digitChar) {
//         case '0':
//         case '1':
//         case '2':
//         case '3':
//         case '4':
//         case '5':
//         case '6':
//         case '7':
//             return digitChar - '0';
//
//         default:
//             *err = EERROR_TYPE_INVALID_OCTAL_LIT;
//             return 0;
//         }
//
//     case ENUMBASE_16:
//         switch (digitChar) {
//         case '0':
//         case '1':
//         case '2':
//         case '3':
//         case '4':
//         case '5':
//         case '6':
//         case '7':
//         case '8':
//         case '9':
//             return digitChar - '0';
//
//         case 'A':
//         case 'B':
//         case 'C':
//         case 'D':
//         case 'E':
//         case 'F':
//             return (digitChar - 'A') + 10;
//
//         case 'a':
//         case 'b':
//         case 'c':
//         case 'd':
//         case 'e':
//         case 'f':
//             return (digitChar - 'a') + 10;
//
//         default:
//             *err = EERROR_TYPE_INVALID_HEX_LIT;
//             return 0;
//         }
//
//     case ENUMBASE_2:
//         switch (digitChar) {
//         case '0':
//         case '1':
//             return digitChar - '0';
//
//         default:
//             *err = EERROR_TYPE_INVALID_BINARY_LIT;
//             return 0;
//         }
//
//     default:
//         ASSERT(false,
//                "This is ridiculous, bro! You should've checked the"
//                " number base enum value to be in the range! You did fuck up!");
//     }
// }

static void scanNumber(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    scanner_resetTokInfo(s);
    scanner_resetDiagnostic(s);

    U8 ch = peekChar(s);
    switch (ch) {
    case '0':
        nextChar(s);
        ch = peekChar(s);
        switch (ch) {
        case 'x':
        case 'X':
            nextChar(s);
            scanHex(s);
            break;
        default:
            if (isOctDigit(ch)) {
                scanOctal(s);
            } else if (isDecDigit(ch)) {
                scanner_setTok(s, ETOKEN_INVALID);
                scanner_setDiagnostic(s, EDIAGNOSTIC_INVALID_OCTAL_LIT);
            } else {
                scanner_setTok(s, ETOKEN_INT_LIT);
                scanner_setTokLexeme(s, new_stringFromLit("0", NULL, NULL));
            }
        }
        break;

    case '.':
        scanFloat(s);
        break;

    default:
        if (isDecDigit(ch)) {
            scanDecimal(s);
        } else {
            ASSERT(false,
                   "You should've peeked a character before going with"
                   " scanning a number, buddy! It shouldn't have gotten here"
                   " at all. So, you probably FUCKED UP something somewhere...");
        }
    }
}

static void scanDecimal(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    scanner_resetTokInfo(s);
    scanner_resetDiagnostic(s);

    if (!isDecDigit(peekChar(s))) {
        ASSERT(false,
               "You should've peeked a character before going with"
               " scanning a decimal, buddy! It shouldn't have gotten here"
               " at all. So, you probably FUCKED UP something somewhere...");
    }

    StringBuilder *digitsSb = new_stringBuilder(NULL, NULL);
    while (isDecDigit(peekChar(s)))
        sb_appendChar(digitsSb, nextChar(s));
    String *const digitsStr = sb_toString(digitsSb);
    scanner_setTok(s, ETOKEN_INT_LIT);
    scanner_setTokLexeme(s, digitsStr);

    // TODO: MOVE THIS TO PARSER, NUMBERS ARE PARSED AND EVALUATED WHILE GENERATING THE AST!
    //
    // U64 exp10 = 1;
    // U64 numVal = 0;
    // for (U64 i = digitsStr->len - 1;; i--) {
    //     const U8 digChar = stringCharAt(digitsStr, i);
    //     const U64 digit = digitFromCharToInt(digChar, ENUMBASE_10, &(s->err));
    //     if (s->err != EERROR_TYPE_NO_ERROR)
    //         return ETOKEN_INVALID;
    //     numVal += (exp10 * digit);
    //     exp10 *= 10;
    //
    //     if (i == 0) // because i is unsigned and always >=0
    //         break;
    // }

    del_stringBuilder(digitsSb);
}

static void scanHex(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // TODO
}

static void scanOctal(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // TODO
}

static void scanFloat(SScanner *const s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // TODO
}

static void scanComment(SScanner *const s, const Bool multiLine)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    scanner_resetDiagnostic(s);
    scanner_resetTokInfo(s);

    if (multiLine) {
        while (!(nextChar(s) == '*' && nextChar(s) == '/')) {
            // TODO: Do we need the comment text later???
        };
    } else {
        while (nextChar(s) != '\n') {
            // TODO: Do we need the comment text later???
        };
    }

    scanner_setTok(s, ETOKEN_COMMENT);
    // TODO: Do we need the comment text later???
    //       If so, set the token lexeme here.
}

static Bool isWhitespace(const U8 ch)
{
    return ch == ' ' || ch == '\n' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r';
}

static Bool isAlpha(const U8 ch)
{
    return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z');
}

static Bool isAlnum(const U8 ch)
{
    return isAlpha(ch) || isDecDigit(ch);
}

static Bool isDecDigit(const U8 ch)
{
    return '0' <= (char)ch && (char)ch <= '9';
}

static Bool isOctDigit(const U8 ch)
{
    return '0' <= (char)ch && (char)ch <= '7';
}

static void skipWhitespace(SScanner *s)
{
    ASSERT(s, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    U8 ch = peekChar(s);
    while (isWhitespace(ch)) {
        nextChar(s); // skip whitespace
        ch = peekChar(s);
    }
}

static Bool initKeywordsMap()
{
    if (keywords == NULL)
        keywords = new_hashmap(NULL, string_memDealloc, NULL, string_hash);

    const U64 kwsCount = eTokenKeywordEnd - eTokenKeywordBeg - 1;
    const String kws[eTokenKeywordEnd - eTokenKeywordBeg - 1] = {
        [ETOKEN_CONST - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"const", .len = strlen("const") },
        [ETOKEN_IF - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"if", .len = strlen("if") },
        [ETOKEN_ELSE - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"else", .len = strlen("else") },
        [ETOKEN_FOR - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"for", .len = strlen("for") },
        [ETOKEN_BREAK - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"break", .len = strlen("break") },
        [ETOKEN_CONTINUE - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"continue", .len = strlen("continue") },
        [ETOKEN_GOTO - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"goto", .len = strlen("goto") },
        [ETOKEN_RETURN - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"return", .len = strlen("return") },
        [ETOKEN_STRUCT - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"struct", .len = strlen("struct") },
        [ETOKEN_UNION - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"union", .len = strlen("union") },
        [ETOKEN_ENUM - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"enum", .len = strlen("enum") },
        [ETOKEN_SWITCH - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"switch", .len = strlen("switch") },
        [ETOKEN_CASE - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"case", .len = strlen("case") },
        [ETOKEN_DEFAULT - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"default", .len = strlen("default") },
        [ETOKEN_WHILE - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"while", .len = strlen("while") },
        [ETOKEN_DO - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"do", .len = strlen("do") },
        [ETOKEN_VOID - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"void", .len = strlen("void") },
        [ETOKEN_STATIC - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"static", .len = strlen("static") },
        [ETOKEN_EXTERN - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"extern", .len = strlen("extern") },
        [ETOKEN_REGISTER - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"register", .len = strlen("register") },
        [ETOKEN_SIGNED - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"signed", .len = strlen("signed") },
        [ETOKEN_UNSIGNED - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"unsigned", .len = strlen("unsigned") },
        [ETOKEN_SIZEOF - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"sizeof", .len = strlen("sizeof") },
        [ETOKEN_TYPEDEF - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"typedef", .len = strlen("typedef") },
        [ETOKEN_VOLATILE - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"volatile", .len = strlen("volatile") },
        [ETOKEN_AUTO - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"auto", .len = strlen("auto") },
        [ETOKEN_INLINE - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"inline", .len = strlen("inline") },
        [ETOKEN_RESTRICT - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"restrict", .len = strlen("restrict") },
        [ETOKEN__COMPLEX - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"_Complex", .len = strlen("_Complex") },
        [ETOKEN__IMAGINARY - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"_Imaginary", .len = strlen("_Imaginary") },
        [ETOKEN__BOOL - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"_Bool", .len = strlen("_Bool") },
        [ETOKEN_CHAR - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"char", .len = strlen("char") },
        [ETOKEN_SHORT - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"short", .len = strlen("short") },
        [ETOKEN_INT - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"int", .len = strlen("int") },
        [ETOKEN_LONG - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"long", .len = strlen("long") },
        [ETOKEN_FLOAT - eTokenKeywordBeg - 1] = { .bytes = (U8 *)"float", .len = strlen("float") },
        [ETOKEN_DOUBLE - eTokenKeywordBeg - 1]
        = { .bytes = (U8 *)"double", .len = strlen("double") }
    };

    if (keywords->size == 0) {
        for (U64 i = 0; i < kwsCount; i++) {
            const String *kw = kws + i;
            const EToken kwTok = (eTokenKeywordBeg + 1 + i);
            hashmapStringEToken_put(keywords, kw, &kwTok);
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////
// HashMap<String, EToken>
///////////////////////////////////////////////////////////////////////////////////
static void printHashMapStringKey(FILE *const out, const U8 *const key)
{
    ASSERT(out != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "FILE");
    ASSERT(key != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "U8");
    fprintf(out, "\"%s\"", key);
}

static void printHashMapETokenVal(FILE *const out, const U8 *const val)
{
    ASSERT(out != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "FILE");
    ASSERT(val != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "U8");

    EToken tok = *((EToken *)val);
    fprintf(out, "%s", eTokenToCStr(tok));
}

void printHashMapStringEToken(const HashMap *const hm)
{
    hashmap_print(hm, stdout, printHashMapStringKey, printHashMapETokenVal);
}

void printKeywordsHashMap()
{
    printHashMapStringEToken(keywords);
}
