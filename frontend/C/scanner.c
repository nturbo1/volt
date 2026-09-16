#include "scanner.h"
#include "base_inc.h"
#include "token.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

// ===============================================================================
// ================================== SScanSrc ===================================
// ===============================================================================
static SScanSrc* initScanSrc(const String* const filepath);
static void delScanSrc(SScanSrc* src);
static void fillScanSrcBuf(SScanSrc* src);
static U8 next(SScanSrc* src);
static U8 peek(SScanSrc* src);

// ===============================================================================
// ================================== SScanner ===================================
// ===============================================================================
static void setTokType(SScanner* s, EToken type);
static void setTokLexeme(SScanner* s, String* lexeme);
static inline void setTok(SToken* tok,
                          const U64 col,
                          const U64 ln,
                          String* lexeme,
                          EToken type);

static EToken scanIdentifier(SScanner* s);
// static EToken scanInt(SScanner* s);
// static EToken scanFloat(SScanner* s);
static EToken scanNumber(SScanner* s);

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

SScanner* new_scanner(const String* const filepath)
{
    SScanSrc* src = initScanSrc(filepath);
    SScanner* s = (SScanner*) malloc(sizeof(SScanner));
    ASSERT(s != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SScanner");
    s->src = src;
    src = NULL;
    s->filepath = new_string(filepath->bytes, filepath->len);
    s->lnOffs = s->colOffs = 0;
    setTok(&(s->tok), 0, 0, NULL, ETOKEN_NO_VALUE);

    initKeywordsMap();

    return s;
}

EToken nextTok(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    skipWhitespace(s);
    setTok(&(s->tok), s->colOffs + 1, s->lnOffs + 1, NULL, ETOKEN_ILLEGAL);
    U8 ch = peekChar(s);
    if ((char) ch == EOF)
    {
        setTokType(s, ETOKEN_EOF);
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
        switch(ch)
        {
        case '(':
	        nextChar(s); setTokType(s, ETOKEN_LPAREN); return s->tok.type;
        case '[':
	        nextChar(s); setTokType(s, ETOKEN_LBRACK); return s->tok.type;
        case '{':
	        nextChar(s); setTokType(s, ETOKEN_LBRACE); return s->tok.type;
        case ')':
	        nextChar(s); setTokType(s, ETOKEN_RPAREN); return s->tok.type;
        case ']':
	        nextChar(s); setTokType(s, ETOKEN_RBRACK); return s->tok.type;
        case '}':
	        nextChar(s); setTokType(s, ETOKEN_RBRACE); return s->tok.type;
	    case ',':
            nextChar(s); setTokType(s, ETOKEN_COMMA); return s->tok.type;
	    case '.':
            {
                nextChar(s);
                ch = peekChar(s);
                if (ch == '.')
                {
                    nextChar(s);
                    ch = peekChar(s);
                    if (ch == '.')
                    {
                        nextChar(s);
                        setTokType(s, ETOKEN_ELLIPSIS); return s->tok.type;
                    }
                    else
                    {
                        setTokType(s, ETOKEN_ILLEGAL);
                        setTokLexeme(s, new_stringFromLit(".."));
                        return s->tok.type;
                    }
                }
                setTokType(s, ETOKEN_PERIOD);
                return s->tok.type;
            }
	    case ':':
            nextChar(s); setTokType(s, ETOKEN_COLON); return s->tok.type;
	    case ';':
            nextChar(s); setTokType(s, ETOKEN_SEMICOLON); return s->tok.type;
        default:
            setTokType(s, ETOKEN_ILLEGAL); return s->tok.type;
        }
    }
}

EToken peekTok(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    return s->tok.type;
}

U8 nextChar(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    U8 ch = next(s->src);

    if ((char) ch != EOF)
    {
        if ((char) ch == '\n')
        {
            s->lnOffs++;
            s->colOffs = 0;
        }
        else
        {
            s->colOffs++;
        }
    }

    return ch;
}

U8 peekChar(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    return peek(s->src);
}

void del_scanner(SScanner* s)
{
    if (s)
    {
        delScanSrc(s->src);
        del_string(s->filepath);
        del_hmap(keywords);
        free(s);
    }
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
    U8 ch = peekChar(s);
    while (isWhitespace(ch))
    {
        nextChar(s); // skip whitespace
        ch = peekChar(s);
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

static inline void setTok(SToken* tok,
                          const U64 col,
                          const U64 ln,
                          String* lexeme,
                          EToken type)
{
    ASSERT(tok != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SToken");
    tok->col = col;
    tok->ln = ln;
    tok->lexeme = lexeme;
    tok->type = type;
}

static void setTokType(SScanner* s, EToken type)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    SToken* tok = &(s->tok);
    setTok(tok, tok->col, tok->ln, tok->lexeme, type);
}

static void setTokLexeme(SScanner* s, String* lexeme)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    SToken* tok = &(s->tok);
    setTok(tok, tok->col, tok->ln, lexeme, tok->type);
}

// ===============================================================================
// ================================== SScanSrc ===================================
// ===============================================================================

static U8 next(SScanSrc* src)
{
    ASSERT(src != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanSrc");
    const U8 ch = src->buf[src->next];

    if ((char) ch != EOF) {
        src->next = ((src->next + 1) % (2 * SCANNER_BUFFER_SIZE));

        // if next is the beginning of one of the buffers
        if (src->next % SCANNER_BUFFER_SIZE == 0) { 
            fillScanSrcBuf(src);
            src->bufEnd = ( (src->next + SCANNER_BUFFER_SIZE) %
                            (2 * SCANNER_BUFFER_SIZE) );
        }
    }

    return ch;
}

static U8 peek(SScanSrc* src)
{
    ASSERT(src != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanSrc");
    return src->buf[src->next];
}

static SScanSrc* initScanSrc(const String* const filepath)
{
    FILE* srcFile = fopen((const char*) filepath->bytes, "r");
    ASSERT(srcFile != NULL,
           "Failed to open file %s\n", (const char*) filepath->bytes);
    SScanSrc* src = (SScanSrc*) malloc(sizeof(SScanSrc));
    ASSERT(src != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SScanSrc");
    src->file = srcFile;
    srcFile = NULL;
    src->bufEnd = SCANNER_BUFFER_SIZE;
    src->next = 0;
    fillScanSrcBuf(src);

    return src;
}

static void delScanSrc(SScanSrc* src)
{
    if (src)
    {
        if (src->file)
            fclose(src->file);
        src->file = NULL;
        free(src);
    }
}

// Fills the current buffer that the scanner is currently processing
// with the next chunk of bytes from the src file.
static void fillScanSrcBuf(SScanSrc* src)
{
    ASSERT(src != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanSrc");
    U8* buf = src->buf + src->next;
    fflush(stdout);
    U64 n = (U64) fread(buf,
                     sizeof(U8),
                     SCANNER_BUFFER_SIZE,
                     src->file);

    if (feof(src->file)) {
        buf[n] = EOF;
    }
}
