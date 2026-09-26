#include "scanner.h"
#include "base_inc.h"
#include "token.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

// HMap<String*, EToken>
static void initKeywordsMap();
static HMap* keywords = NULL;

///////////////////////////////////////////////////////////////////////////////////
// HMap<String, EToken>
///////////////////////////////////////////////////////////////////////////////////

static void hmapStringToEToken_put(HMap* const hm, const String* const key, const EToken val)
{
    ASSERT(key != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "String");
    ASSERT(key->len > 0, "Empty string can't be a hashmap key.");
    hmap_put(hm, key->bytes, key->len + 1, (U8*) &val, sizeof(EToken));
}

static EToken hmapStringToEToken_get(HMap* const hm, const String* const key)
{
    ASSERT(key != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "String");
    ASSERT(key->len > 0, "Empty string can't be a hashmap key.");
    MapElem* elem = hmap_get(hm, key->bytes, key->len + 1);
    if (elem != NULL)
    {
        ASSERT_DBG(elem->valSize == sizeof(EToken),
                   "HMap element value size doesn't match EToken size.");
        return *(EToken*)(elem->val);
    }

    return eTokenEnd;
}

static void printHMapStringKey(FILE* const out, const U8* const key)
{
    ASSERT(out != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "FILE");
    ASSERT(key != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "U8");
    fprintf(out, "\"%s\"", key);
}

static void printHMapETokenVal(FILE* const out, const U8* const val)
{
    ASSERT(out != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "FILE");
    ASSERT(val != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "U8");

    EToken tok = *((EToken*) val);
    fprintf(out, "%s", eTokenToCStr(tok));
}

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
static inline void setTokBase(UToken* tok, const U64 col, const U64 ln, EToken type);
static void setTokNumber(UToken* tok,
                         const U64 col,
                         const U64 ln,
                         EToken type,
                         const U64 val);
static inline void setTokIdent(UToken* tok,
                               const U64 col,
                               const U64 ln,
                               EToken type,
                               String* lexeme);

static EToken scanIdentifier(SScanner* s);
// static EToken scanInt(SScanner* s);
static EToken scanNumber(SScanner* s);
static EToken scanOctal(SScanner* s);
static EToken scanHex(SScanner* s);
static EToken scanDecimal(SScanner* s);
static EToken scanFloat(SScanner* s);
static EToken scanComment(SScanner* s, const bool multiLine);

static bool isWhitespace(const U8 ch);
static bool isAlpha(const U8 ch);
static bool isAlnum(const U8 ch);
static bool isDecDigit(const U8 ch);
static bool isOctDigit(const U8 ch);
static void skipWhitespace(SScanner* s);

typedef enum ENumBase
{
    eNumBaseBeg,
    ENUMBASE_10,
    ENUMBASE_8,
    ENUMBASE_16,
    ENUMBASE_2,
    eNumBaseEnd
}
ENumBase;

// Converts a given digit character in a given number base
// to its integral value.
// For instance:
//     decimal:     '9' -> 9, '0' -> 0, '3' -> 3, ...
//     hexadecimal: '2' -> 2, '0' -> 0, 'a' -> 10, 'F' -> 15, ...
//     octal:       '4' -> 4, '7' -> 7, '0' -> 0, ...
//     binary:      '0' -> 0, '1' -> 1
inline static U64 digitFromCharToInt(const U8 digitChar, const ENumBase base, EErrorType* const err)
{
    *err = EERROR_TYPE_NO_ERROR;
    ASSERT(eNumBaseBeg < base && base < eNumBaseEnd,
           "Number base enum value is out of range.");

    switch(base)
    {
    case ENUMBASE_10:
        switch(digitChar)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return digitChar - '0';

        default:
            *err = EERROR_TYPE_INVALID_DECIMAL_LIT;
            return 0;
        }

    case ENUMBASE_8:
        switch(digitChar)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            return digitChar - '0';

        default:
            *err = EERROR_TYPE_INVALID_OCTAL_LIT;
            return 0;
        }

    case ENUMBASE_16:
        switch(digitChar)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return digitChar - '0';

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            return (digitChar - 'A') + 10;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            return (digitChar - 'a') + 10;

        default:
            *err = EERROR_TYPE_INVALID_HEX_LIT;
            return 0;
        }

    case ENUMBASE_2:
        switch(digitChar)
        {
        case '0':
        case '1':
            return digitChar - '0';

        default:
            *err = EERROR_TYPE_INVALID_BINARY_LIT;
            return 0;
        }

    default:
        ASSERT(false, "This is ridiculous, bro! You should've checked the"
               " number base enum value to be in the range! You did fuck up!");
    }
}

SScanner* new_scanner(const String* const filepath)
{
    SScanSrc* src = initScanSrc(filepath);
    SScanner* s = (SScanner*) malloc(sizeof(SScanner));
    ASSERT(s != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SScanner");
    s->src = src;
    src = NULL;
    s->filepath = new_string(filepath->bytes, filepath->len);
    s->lnOffs = s->colOffs = 0;
    setTokBase(&(s->tok), 0, 0, ETOKEN_NO_VALUE);
    s->err = EERROR_TYPE_NO_ERROR;

    initKeywordsMap();

    return s;
}

EToken nextTok(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->err = EERROR_TYPE_NO_ERROR;
    skipWhitespace(s);
    const U64 tokCol = s->colOffs + 1;
    const U64 tokLn = s->lnOffs + 1;
    U8 ch = peekChar(s);
    if ((char) ch == EOF)
    {
        setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_EOF);
        return ETOKEN_EOF;
    }

    if(isDecDigit(ch))
    {
        return scanNumber(s);
    }
    else
    {
        switch(ch)
        {
        case '(':
	        nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_LPAREN);
            return ETOKEN_LPAREN;

        case '[':
	        nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_LBRACK);
            return ETOKEN_LBRACK;

        case '{':
	        nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_LBRACE);
            return ETOKEN_LBRACE;

        case ')':
	        nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_RPAREN);
            return ETOKEN_RPAREN;

        case ']':
	        nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_RBRACK);
            return ETOKEN_RBRACK;

        case '}':
	        nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_RBRACE);
            return ETOKEN_RBRACE;

	    case ',':
            nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_COMMA);
            return ETOKEN_COMMA;

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
                        setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_ELLIPSIS);
                        return ETOKEN_ELLIPSIS;
                    }
                    else
                    {
                        setTokIdent(&(s->tok),
                                    tokCol,
                                    tokLn,
                                    ETOKEN_INVALID_IDENT,
                                    new_stringFromLit(".."));
                        s->err = EERROR_TYPE_INVALID_IDENT;
                        return ETOKEN_INVALID_IDENT;
                    }
                }
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_PERIOD);
                return ETOKEN_PERIOD;
            }

	    case ':':
            nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_COLON);
            return ETOKEN_COLON;

	    case ';':
            nextChar(s);
            setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_SEMICOLON);
            return ETOKEN_SEMICOLON;

        case '*':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_MUL_ASSIGN);
                return ETOKEN_MUL_ASSIGN;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_ASTERISK);
                return ETOKEN_ASTERISK;
            }

        case '/':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '/':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, scanComment(s, false));
                return s->tok.base.type;

            case '*':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, scanComment(s, true));
                return s->tok.base.type;

            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_QUO_ASSIGN);
                return ETOKEN_QUO_ASSIGN;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_QUO);
                return ETOKEN_QUO;
            }

        case '+':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '+':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_INC);
                return ETOKEN_INC;

            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_ADD_ASSIGN);
                return ETOKEN_ADD_ASSIGN;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_ADD);
                return ETOKEN_ADD;
            }

        case '-':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '-':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_DEC);
                return ETOKEN_DEC;

            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_SUB_ASSIGN);
                return ETOKEN_SUB_ASSIGN;

            case '>':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_RARROW);
                return ETOKEN_RARROW;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_SUB);
                return ETOKEN_SUB;
            }

        case '%':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_REM_ASSIGN);
                return ETOKEN_REM_ASSIGN;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_REM);
                return ETOKEN_REM;
            }

        case '&':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '^':
                nextChar(s);
                ch = peekChar(s);
                switch(ch)
                {
                case '=':
                    nextChar(s);
                    setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_AND_NOT_ASSIGN);
                    return ETOKEN_AND_NOT_ASSIGN;

                default:
                    setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_AND_NOT);
                    return ETOKEN_AND_NOT;
                }

            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_AND_ASSIGN);
                return ETOKEN_AND_ASSIGN;

            case '&':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_LAND);
                return ETOKEN_LAND;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_AND);
                return ETOKEN_AND;
            }

        case '|':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '|':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_LOR);
                return ETOKEN_LOR;

            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_OR_ASSIGN);
                return ETOKEN_OR_ASSIGN;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_OR);
                return ETOKEN_OR;
            }

        case '^':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_XOR_ASSIGN);
                return ETOKEN_XOR_ASSIGN;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_XOR);
                return ETOKEN_XOR;
            }

        case '<':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_LEQ);
                return ETOKEN_LEQ;

            case '<':
                nextChar(s);
                ch = peekChar(s);
                switch(ch)
                {
                case '=':
                    nextChar(s);
                    setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_SHL_ASSIGN);
                    return ETOKEN_SHL_ASSIGN;

                default:
                    setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_SHL);
                    return ETOKEN_SHL;
                }

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_LSS);
                return ETOKEN_LSS;
            }

        case '>':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_GEQ);
                return ETOKEN_GEQ;

            case '>':
                nextChar(s);
                ch = peekChar(s);
                switch(ch)
                {
                case '=':
                    nextChar(s);
                    setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_SHR_ASSIGN);
                    return ETOKEN_SHR_ASSIGN;

                default:
                    setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_SHR);
                    return ETOKEN_SHR;
                }

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_GTR);
                return ETOKEN_GTR;
            }

        case '=':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_EQL);
                return ETOKEN_EQL;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_ASSIGN);
                return ETOKEN_ASSIGN;
            }

        case '!':
            nextChar(s);
            ch = peekChar(s);
            switch(ch)
            {
            case '=':
                nextChar(s);
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_NEQ);
                return ETOKEN_NEQ;

            default:
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_NOT);
                return ETOKEN_NOT;
            }

        default:
            return scanIdentifier(s);
        }
    }
}

EToken peekTok(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    return s->tok.base.type;
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
        keywords = NULL;
        free(s);
    }
}

static EToken scanIdentifier(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->err = EERROR_TYPE_NO_ERROR;
    const U64 tokCol = s->colOffs + 1;
    const U64 tokLn = s->lnOffs + 1;
    U8 ch = peekChar(s);
    if (!isAlpha(ch) && ch != '_')
    {
        setTokIdent(&(s->tok), tokCol, tokLn, ETOKEN_INVALID_IDENT, NULL);
        s->err = EERROR_TYPE_INVALID_IDENT;
        return ETOKEN_INVALID_IDENT;
    }

    SStringBuilder* identSb = new_stringBuilder();
    while (isAlnum(ch) || ch == '_')
    {
        sb_appendChar(identSb, nextChar(s));
        ch = peekChar(s);
    }

    String* ident = sb_toString(identSb);
    EToken identTokType = hmapStringToEToken_get(keywords, ident);
    if(identTokType != eTokenEnd)
    {
        setTokBase(&(s->tok), tokCol, tokLn, identTokType);
        goto defer;
    }
    setTokIdent(&(s->tok), tokCol, tokLn, ETOKEN_IDENT, ident);

defer:
    del_string(ident);
    del_stringBuilder(identSb);
    return s->tok.base.type;
}

static EToken scanNumber(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->err = EERROR_TYPE_NO_ERROR;
    const U64 tokCol = s->colOffs + 1;
    const U64 tokLn = s->lnOffs + 1;
    U8 ch = peekChar(s);
    switch(ch)
    {
    case '0':
        nextChar(s);
        ch = peekChar(s);
        switch(ch)
        {
        case 'x':
        case 'X':
            nextChar(s);
            return scanHex(s);
        default:
            if (isOctDigit(ch))
            {
                return scanOctal(s);
            }
            else if (isDecDigit(ch))
            {
                setTokBase(&(s->tok), tokCol, tokLn, ETOKEN_INVALID_INT_LIT);
                s->err = EERROR_TYPE_INVALID_OCTAL_LIT;
                return ETOKEN_INVALID_INT_LIT;
            }
            else
            {
                setTokNumber(&(s->tok), tokCol, tokLn, ETOKEN_INT_LIT, 0);
                return ETOKEN_INT_LIT;
            }
        }

    case '.':
        return scanFloat(s);

    default:
        if (isDecDigit(ch))
            return scanDecimal(s);
        else
            ASSERT(false,
                   "You should've peeked a character before going with"
                   " scanning a number, buddy! It shouldn't have gotten here"
                   " at all. So, you probably FUCKED UP something somewhere...");
    }
}

static EToken scanDecimal(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    s->err = EERROR_TYPE_NO_ERROR;
    if (!isDecDigit(peekChar(s)))
        ASSERT(false,
               "You should've peeked a character before going with"
               " scanning a decimal, buddy! It shouldn't have gotten here"
               " at all. So, you probably FUCKED UP something somewhere...");
    
    const U64 tokCol = s->colOffs + 1;
    const U64 tokLn = s->lnOffs + 1;
    SStringBuilder* digitsSb = new_stringBuilder();
    while(isDecDigit(peekChar(s)))
        sb_appendChar(digitsSb, nextChar(s));
    String* digitsStr = sb_toString(digitsSb);
    U64 exp10 = 1;
    U64 numVal = 0;
    for (U64 i = digitsStr->len - 1;; i--)
    {
        const U8 digChar = stringCharAt(digitsStr, i);
        const U64 digit = digitFromCharToInt(digChar, ENUMBASE_10, &(s->err));
        if (s->err != EERROR_TYPE_NO_ERROR)
            return ETOKEN_INVALID_INT_LIT;
        numVal += (exp10 * digit);
        exp10 *= 10;

        if (i == 0) // because i is unsigned and always >=0
            break;
    }

    del_stringBuilder(digitsSb);
    del_string(digitsStr);
    setTokNumber(&(s->tok), tokCol, tokLn, ETOKEN_INT_LIT, numVal);
    return ETOKEN_INT_LIT;
}

static EToken scanHex(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // TODO
    return ETOKEN_INVALID_INT_LIT;
}

static EToken scanOctal(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // TODO
    return ETOKEN_INVALID_INT_LIT;
}

static EToken scanFloat(SScanner* s)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    // TODO
    return ETOKEN_INVALID_FLOAT_LIT;
}

static EToken scanComment(SScanner* s, const bool multiLine)
{
    ASSERT(s != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SScanner");
    if (multiLine)
        while ( !(nextChar(s) == '*' && nextChar(s) == '/') );
    else
        while (nextChar(s) != '\n');

    return ETOKEN_COMMENT;
}

static bool isWhitespace(const U8 ch)
{
    return ch == ' ' || ch == '\n' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r';
}

static bool isAlpha(const U8 ch)
{
    return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z');
}

static bool isAlnum(const U8 ch)
{
    return isAlpha(ch) || isDecDigit(ch);
}

static bool isDecDigit(const U8 ch)
{
    return '0' <= (char) ch && (char) ch <= '9';
}

static bool isOctDigit(const U8 ch)
{
    return '0' <= (char) ch && (char) ch <= '7';
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

static void initKeywordsMap()
{
    if (keywords == NULL)
        keywords = new_hmap();

    const U64 kwsCount = eTokenKeywordEnd - eTokenKeywordBeg - 1;
    const String kws[eTokenKeywordEnd - eTokenKeywordBeg - 1] = {
        [ETOKEN_CONST - eTokenKeywordBeg - 1] = { .bytes = (U8*) "const", .len = strlen("const") },
        [ETOKEN_IF - eTokenKeywordBeg - 1] = { .bytes = (U8*) "if", .len = strlen("if") },
        [ETOKEN_ELSE - eTokenKeywordBeg - 1] = { .bytes = (U8*) "else", .len = strlen("else") },
        [ETOKEN_FOR - eTokenKeywordBeg - 1] = { .bytes = (U8*) "for", .len = strlen("for") },
        [ETOKEN_BREAK - eTokenKeywordBeg - 1] = { .bytes = (U8*) "break", .len = strlen("break") },
        [ETOKEN_CONTINUE - eTokenKeywordBeg - 1] = { .bytes = (U8*) "continue", .len = strlen("continue") },
        [ETOKEN_GOTO - eTokenKeywordBeg - 1] = { .bytes = (U8*) "goto", .len = strlen("goto") },
        [ETOKEN_RETURN - eTokenKeywordBeg - 1] = { .bytes = (U8*) "return", .len = strlen("return") },
        [ETOKEN_STRUCT - eTokenKeywordBeg - 1] = { .bytes = (U8*) "struct", .len = strlen("struct") },
        [ETOKEN_UNION - eTokenKeywordBeg - 1] = { .bytes = (U8*) "union", .len = strlen("union") },
        [ETOKEN_ENUM - eTokenKeywordBeg - 1] = { .bytes = (U8*) "enum", .len = strlen("enum") },
        [ETOKEN_SWITCH - eTokenKeywordBeg - 1] = { .bytes = (U8*) "switch", .len = strlen("switch") },
        [ETOKEN_CASE - eTokenKeywordBeg - 1] = { .bytes = (U8*) "case", .len = strlen("case") },
        [ETOKEN_DEFAULT - eTokenKeywordBeg - 1] = { .bytes = (U8*) "default", .len = strlen("default") },
        [ETOKEN_WHILE - eTokenKeywordBeg - 1] = { .bytes = (U8*) "while", .len = strlen("while") },
        [ETOKEN_DO - eTokenKeywordBeg - 1] = { .bytes = (U8*) "do", .len = strlen("do") },
        [ETOKEN_VOID - eTokenKeywordBeg - 1] = { .bytes = (U8*) "void", .len = strlen("void") },
        [ETOKEN_STATIC - eTokenKeywordBeg - 1] = { .bytes = (U8*) "static", .len = strlen("static") },
        [ETOKEN_EXTERN - eTokenKeywordBeg - 1] = { .bytes = (U8*) "extern", .len = strlen("extern") },
        [ETOKEN_REGISTER - eTokenKeywordBeg - 1] = { .bytes = (U8*) "register", .len = strlen("register") },
        [ETOKEN_SIGNED - eTokenKeywordBeg - 1] = { .bytes = (U8*) "signed", .len = strlen("signed") },
        [ETOKEN_UNSIGNED - eTokenKeywordBeg - 1] = { .bytes = (U8*) "unsigned", .len = strlen("unsigned") },
        [ETOKEN_SIZEOF - eTokenKeywordBeg - 1] = { .bytes = (U8*) "sizeof", .len = strlen("sizeof") },
        [ETOKEN_TYPEDEF - eTokenKeywordBeg - 1] = { .bytes = (U8*) "typedef", .len = strlen("typedef") },
        [ETOKEN_VOLATILE - eTokenKeywordBeg - 1] = { .bytes = (U8*) "volatile", .len = strlen("volatile") },
        [ETOKEN_AUTO - eTokenKeywordBeg - 1] = { .bytes = (U8*) "auto", .len = strlen("auto") },
        [ETOKEN_INLINE - eTokenKeywordBeg - 1] = { .bytes = (U8*) "inline", .len = strlen("inline") },
        [ETOKEN_RESTRICT - eTokenKeywordBeg - 1] = { .bytes = (U8*) "restrict", .len = strlen("restrict") },
        [ETOKEN__COMPLEX - eTokenKeywordBeg - 1] = { .bytes = (U8*) "_Complex", .len = strlen("_Complex") },
        [ETOKEN__IMAGINARY - eTokenKeywordBeg - 1] = { .bytes = (U8*) "_Imaginary", .len = strlen("_Imaginary") },
        [ETOKEN__BOOL - eTokenKeywordBeg - 1] = { .bytes = (U8*) "_Bool", .len = strlen("_Bool") },
        [ETOKEN_CHAR - eTokenKeywordBeg - 1] = { .bytes = (U8*) "char", .len = strlen("char") },
        [ETOKEN_SHORT - eTokenKeywordBeg - 1] = { .bytes = (U8*) "short", .len = strlen("short") },
        [ETOKEN_INT - eTokenKeywordBeg - 1] = { .bytes = (U8*) "int", .len = strlen("int") },
        [ETOKEN_LONG - eTokenKeywordBeg - 1] = { .bytes = (U8*) "long", .len = strlen("long") },
        [ETOKEN_FLOAT - eTokenKeywordBeg - 1] = { .bytes = (U8*) "float", .len = strlen("float") },
        [ETOKEN_DOUBLE - eTokenKeywordBeg - 1] = { .bytes = (U8*) "double", .len = strlen("double") }
    };

    if (keywords->size == 0)
    {
        for (U64 i = 0; i < kwsCount; i++)
        {
            const String* kw = kws + i;
            hmapStringToEToken_put(keywords, kw, (EToken) (eTokenKeywordBeg + 1 + i));
        }
    }
}

static inline void setTokBase(UToken* tok, const U64 col, const U64 ln, EToken type)
{
    ASSERT(tok != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "UToken");
    tok->base.col = col;
    tok->base.ln = ln;
    tok->base.type = type;
}

static inline void setTokIdent(UToken* tok,
                               const U64 col,
                               const U64 ln,
                               EToken type,
                               String* lexeme)
{
    setTokBase(tok, col, ln, type);
    tok->ident.lexeme = lexeme;
}

static void setTokNumber(UToken* tok,
                         const U64 col,
                         const U64 ln,
                         EToken type,
                         const U64 val)
{
    setTokBase(tok, col, ln, type);
    tok->number.val = val;
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

///////////////////////////////////////////////////////////////////////////////////
// HMap<String, EToken>
///////////////////////////////////////////////////////////////////////////////////
void printHMapStringEToken(const HMap* const hm)
{
    hmap_print(hm, stdout, printHMapStringKey, printHMapETokenVal);
}

void printKeywordsHMap()
{
    printHMapStringEToken(keywords);
}
