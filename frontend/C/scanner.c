#include "scanner.h"
#include "base_inc.h"
#include "token.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

static EToken scanIdentifier(SScanner* s);
// static EToken scanInt(SScanner* s);
// static EToken scanFloat(SScanner* s);
static EToken scanNumber(SScanner* s);

static void fillBuf(SScanner* s);
static bool isWhitespace(const U8 ch);
static bool isAlpha(const U8 ch);
// static bool isAlnum(const U8 ch);
static bool isDecDigit(const U8 ch);
static void skipWhitespace(SScanner* s);

///////////////////////////////////////////////////////////////////////////////////
// HMap<String, EToken>
///////////////////////////////////////////////////////////////////////////////////
// static EToken hmapStringToEToken_get(HMap* const hm, String* key);
static void hmapStringToEToken_put(HMap* const hm, String* key, EToken val);
static void initKeywordsMap();

// HMap<String*, EToken>
static HMap* keywords = NULL;

SScanner* new_scanner(String* filepath)
{
    FILE* srcFile = fopen((const char*) filepath->bytes, "r");
    ASSERT(srcFile != NULL, "Failed to open file %s\n", (const char*) filepath->bytes);
    SScanner* s = (SScanner*) malloc(sizeof(SScanner));
    ASSERT(s != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SScanner");
    s->file = srcFile;
    srcFile = NULL;
    s->filepath = new_string(filepath->bytes, filepath->len);
    s->bufEnd = SCANNER_BUFFER_SIZE;
    s->nextChIdx = 0;
    s->lnOffs = s->colOffs = s->tokLn = s->tokCol = 0;
    s->tokLexeme = NULL;
    fillBuf(s);
    s->tok = nextTok(s); // may update SScanner obj, s

    initKeywordsMap();

    return s;
}

EToken nextTok(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->tokLexeme = NULL;
    s->tok = ETOKEN_ILLEGAL;

    skipWhitespace(s);
    s->tokLn = s->lnOffs + 1;
    s->tokCol = s->colOffs + 1;
    U8 ch = peek(s);
    if ((char) ch == EOF)
    {
        s->tokLexeme = NULL;
        s->tok = ETOKEN_EOF;
        return ETOKEN_EOF;
    }

    if (isAlpha(ch))
    {
        return scanIdentifier(s);
    }
    else if(isDecDigit(ch))
    {
        return scanNumber(s);
    }
    else
    {
        s->tokLexeme = NULL;
        switch(ch)
        {
        case '(':
	        next(s); s->tok = ETOKEN_LPAREN; return ETOKEN_LPAREN;
        case '[':
	        next(s); s->tok = ETOKEN_LBRACK; return ETOKEN_LBRACK;
        case '{':
	        next(s); s->tok = ETOKEN_LBRACE; return ETOKEN_LBRACE;
        case ')':
	        next(s); s->tok = ETOKEN_RPAREN; return ETOKEN_RPAREN;
        case ']':
	        next(s); s->tok = ETOKEN_RBRACK; return ETOKEN_RBRACK;
        case '}':
	        next(s); s->tok = ETOKEN_RBRACE; return ETOKEN_RBRACE;
	    case ',':
            next(s); s->tok = ETOKEN_COMMA; return ETOKEN_COMMA;
	    case '.':
            {
                next(s);
                ch = peek(s);
                if (ch == '.')
                {
                    next(s);
                    ch = peek(s);
                    if (ch == '.')
                    {
                        next(s);
                        s->tok = ETOKEN_ELLIPSIS;
                        return ETOKEN_ELLIPSIS;
                    }
                    else
                    {
                        s->tok = ETOKEN_ILLEGAL;
                        s->tokLexeme = new_stringFromLit("..");
                        return ETOKEN_ILLEGAL;
                    }
                }
                s->tok = ETOKEN_PERIOD;
                return ETOKEN_PERIOD;
            }
	    case ':':
            next(s); s->tok = ETOKEN_COLON; return ETOKEN_COLON;
	    case ';':
            next(s); s->tok = ETOKEN_SEMICOLON; return ETOKEN_SEMICOLON;
        default:
            s->tok = ETOKEN_ILLEGAL; return ETOKEN_ILLEGAL;
        }
    }
}

EToken peekTok(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    return s->tok;
}

U8 next(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    const U8 ch = s->buf[s->nextChIdx];

    if ((char) ch != EOF) {
        s->nextChIdx = ((s->nextChIdx + 1) % (2 * SCANNER_BUFFER_SIZE));

        // next is the beginning of one of the buffers
        if (s->nextChIdx % SCANNER_BUFFER_SIZE == 0) { 
            fillBuf(s);
            s->bufEnd = ((s->nextChIdx + SCANNER_BUFFER_SIZE) % (2 * SCANNER_BUFFER_SIZE));
        }

        if (ch == '\n') {
            s->lnOffs++;
            s->colOffs = 0;
        } else {
            s->colOffs++;
        }
    }

    return ch;
}

U8 peek(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    return s->buf[s->nextChIdx];
}

void del_scanner(SScanner* s)
{
    if (s->file != NULL)
        fclose(s->file);

    del_string(s->filepath);
    s->filepath = NULL;
    del_string(s->tokLexeme);
    s->tokLexeme = NULL;

    del_hmap(keywords);

    free(s);
}

static EToken scanIdentifier(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // TODO
    return 0;
}

static EToken scanNumber(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // TODO
    return 0;
}

// Fills the current buffer that the scanner is currently processing
// with the next chunk of bytes from the src file.
static void fillBuf(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    U8* buf = s->buf + s->nextChIdx;
    fflush(stdout);
    U64 n = (U64) fread(buf,
                     sizeof(U8),
                     SCANNER_BUFFER_SIZE,
                     s->file);

    if (feof(s->file)) {
        buf[n] = EOF;
    }
}

static bool isWhitespace(const U8 ch)
{
    return ch == ' ' || ch == '\n' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r';
}

static bool isAlpha(const U8 ch)
{
    return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z');
}

// static bool isAlnum(const U8 ch)
// {
//     return isAlpha(ch) || isDecDigit(ch);
// }

static bool isDecDigit(const U8 ch)
{
    return '0' <= (char) ch && (char) ch <= '9';
}

static void skipWhitespace(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    U8 ch = peek(s);
    while (isWhitespace(ch))
    {
        next(s); // skip whitespace
        ch = peek(s);
    }
}

///////////////////////////////////////////////////////////////////////////////////
// HMap<String, EToken>
///////////////////////////////////////////////////////////////////////////////////

// static EToken hmapStringToEToken_get(HMap* const hm, String* key)
// {
//     ASSERT(key != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "String");
//     MapElem* elem = hmap_get(hm, key->bytes, key->len);
//     if (elem != NULL)
//     {
//         ASSERT_DBG(elem->valSize == sizeof(EToken), "HMap element value size doesn't match EToken size.");
//         return *(EToken*)(elem->val);
//     }
//
//     return eTokenEnd;
// }

static void hmapStringToEToken_put(HMap* const hm, String* key, EToken val)
{
    ASSERT(key != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "String");
    hmap_put(hm, key->bytes, key->len, (U8*) &val, sizeof(EToken));
}

static void initKeywordsMap()
{
    if (keywords != NULL)
    {
        keywords = new_hmap();
        hmapStringToEToken_put(keywords, new_stringFromLit("const"),       ETOKEN_CONST);
        hmapStringToEToken_put(keywords, new_stringFromLit("if"),          ETOKEN_IF);
        hmapStringToEToken_put(keywords, new_stringFromLit("else"),        ETOKEN_ELSE);
        hmapStringToEToken_put(keywords, new_stringFromLit("for"),         ETOKEN_FOR);
        hmapStringToEToken_put(keywords, new_stringFromLit("break"),       ETOKEN_BREAK);
        hmapStringToEToken_put(keywords, new_stringFromLit("continue"),    ETOKEN_CONTINUE);
        hmapStringToEToken_put(keywords, new_stringFromLit("goto"),        ETOKEN_GOTO);
        hmapStringToEToken_put(keywords, new_stringFromLit("return"),      ETOKEN_RETURN);
        hmapStringToEToken_put(keywords, new_stringFromLit("struct"),      ETOKEN_STRUCT);
        hmapStringToEToken_put(keywords, new_stringFromLit("union"),       ETOKEN_UNION);
        hmapStringToEToken_put(keywords, new_stringFromLit("enum"),        ETOKEN_ENUM);
        hmapStringToEToken_put(keywords, new_stringFromLit("switch"),      ETOKEN_SWITCH);
        hmapStringToEToken_put(keywords, new_stringFromLit("case"),        ETOKEN_CASE);
        hmapStringToEToken_put(keywords, new_stringFromLit("default"),     ETOKEN_DEFAULT);
        hmapStringToEToken_put(keywords, new_stringFromLit("while"),       ETOKEN_WHILE);
        hmapStringToEToken_put(keywords, new_stringFromLit("do"),          ETOKEN_DO);
        hmapStringToEToken_put(keywords, new_stringFromLit("void"),        ETOKEN_VOID);
        hmapStringToEToken_put(keywords, new_stringFromLit("static"),      ETOKEN_STATIC);
        hmapStringToEToken_put(keywords, new_stringFromLit("extern"),      ETOKEN_EXTERN);
        hmapStringToEToken_put(keywords, new_stringFromLit("register"),    ETOKEN_REGISTER);
        hmapStringToEToken_put(keywords, new_stringFromLit("signed"),      ETOKEN_SIGNED);
        hmapStringToEToken_put(keywords, new_stringFromLit("unsigned"),    ETOKEN_UNSIGNED);
        hmapStringToEToken_put(keywords, new_stringFromLit("sizeof"),      ETOKEN_SIZEOF);
        hmapStringToEToken_put(keywords, new_stringFromLit("typedef"),     ETOKEN_TYPEDEF);
        hmapStringToEToken_put(keywords, new_stringFromLit("volatile"),    ETOKEN_VOLATILE);
        hmapStringToEToken_put(keywords, new_stringFromLit("auto"),        ETOKEN_AUTO); 
        hmapStringToEToken_put(keywords, new_stringFromLit("inline"),      ETOKEN_INLINE);
        hmapStringToEToken_put(keywords, new_stringFromLit("restrict"),    ETOKEN_RESTRICT);
        hmapStringToEToken_put(keywords, new_stringFromLit("_complex"),    ETOKEN__COMPLEX);
        hmapStringToEToken_put(keywords, new_stringFromLit("_imaginary"),  ETOKEN__IMAGINARY);
        hmapStringToEToken_put(keywords, new_stringFromLit("_bool"),       ETOKEN__BOOL);
        hmapStringToEToken_put(keywords, new_stringFromLit("char"),        ETOKEN_CHAR);
        hmapStringToEToken_put(keywords, new_stringFromLit("short"),       ETOKEN_SHORT);
        hmapStringToEToken_put(keywords, new_stringFromLit("int"),         ETOKEN_INT);
        hmapStringToEToken_put(keywords, new_stringFromLit("long"),        ETOKEN_LONG);
        hmapStringToEToken_put(keywords, new_stringFromLit("float"),       ETOKEN_FLOAT);
        hmapStringToEToken_put(keywords, new_stringFromLit("double"),      ETOKEN_DOUBLE);
    }
}

