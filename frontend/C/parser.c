#include "parser.h"

#include "base_inc.h"
#include "diangostics.h"
#include "scanner.h"
#include "token.h"

#include <stdbool.h>
#include <stdlib.h>

#define PARSER_TOKEN_LOOKAHEAD_SIZE 5
#define PARSER_ERROR_COUNT_LIMIT 5

#define PARSER_TOKEN_WINDOW_NOT_INITIALIZED                                                        \
    "Parser token look-ahead ring buffer is not initialized!"
#define PARSER_INCORRECT_TOKEN_WINDOW_BUFFER_SIZE                                                  \
    "Incorrect parser token window buffer size, the size MUST be an even number."

Bool tokWind_advance(STokWind *const tw, const UToken *const tok)
{
    ASSERT(tw != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "STokWind");
    ASSERT(tok != NULL, "Next token MUST NOT be NULL!");
    ASSERT_DBG(tw->toks[tw->end].base.type != ETOKEN_EOF,
               "You have already reached the end of the file, bro!");

    if (tw->toks[tw->end].base.type == ETOKEN_EOF)
        return false;

    tw->end = ((tw->end + 1) % tw->len);
    copyBytesFromTo((const U8 *const)tok, sizeof(UToken), (U8 *)(tw->toks + tw->end));
    tw->next++;

    return true;
}

const UToken *tokWind_getNext(const STokWind *const tw)
{
    ASSERT(tw, NULL_POINTER_ERROR_MSG_FORMAT, "STokWind");
    ASSERT(tw->toks, "Parser tokens window buffer can't be NULL!");
    ASSERT_DBG(tw->len > 0, "Parser tokens window buffer can't have 0 length!");
    ASSERT(tw->next <= tw->end, "Parser tokens window buffer: next exceeded end!");

    return &(tw->toks[tw->next]);
}

// =====================================================================
// ============================== SParser ==============================
// =====================================================================
/*
 * - Scans the next `n` tokens and appends them all in the
 *   order they have been scanned to the window buffer.
 *
 * - Nullifies the remaining slots of the buffer.
 *
 * - The window buffer should like this after the
 *   initialization:
 *
 *       start, next                     end
 *            0         1     2    ...  n - 1   n    n + 2   ...   2n + 1
 *     |             | tok | tok | ... | tok | null | null | ... | null |
 *
 */
static void initParserTokWind(SParser *parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->scanner != NULL,
           "Init the scanner before initializing the parser token window!");

    STokWind *tokWind = (STokWind *)malloc(sizeof(STokWind));
    ASSERT(tokWind != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "STokWind");
    tokWind->len = (2 * PARSER_TOKEN_LOOKAHEAD_SIZE);
    tokWind->toks = NULL;
    tokWind->toks = (UToken *)malloc(sizeof(UToken) * tokWind->len);
    ASSERT(tokWind->toks != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "UToken array");

    // Nullify the tokens window buffer
    for (U64 i = 0; i < tokWind->len; i++) {
        tokWind->toks[i].base.col = 0;
        tokWind->toks[i].base.ln = 0;
        tokWind->toks[i].ident.lexeme = NULL;
        tokWind->toks[i].number.val = 0;
    }
    tokWind->end = 0;
    tokWind->next = 0;

    ASSERT_DBG(tokWind->len % 2 == 0, PARSER_INCORRECT_TOKEN_WINDOW_BUFFER_SIZE);
    const U64 tokLookAheadSize = tokWind->len / 2;
    for (U64 i = 0; i < tokLookAheadSize; i++) {
        nextTok(parser->scanner);
        const UToken *const tok = &(parser->scanner->tok);
        const Bool isEof = tokWind_advance(parser->tokWind, tok);

        if (parser->scanner->err != EERROR_TYPE_NO_ERROR) {
            SError err;
            initError(&err, parser->scanner->err, tok);
            eh_handle(parser->eh, &err);
        }

        if (isEof)
            break;
    }

    parser->tokWind = tokWind;
}

SParser *new_parser(const String *const filepath)
{
    SScanner *scanner = new_scanner(filepath);
    ASSERT(scanner != NULL, "Failed to create SScanner.");
    SParser *parser = (SParser *)malloc(sizeof(SParser));
    ASSERT(parser != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SParser");
    parser->scanner = scanner;

    initParserTokWind(parser);

    parser->eh = new_errHandler(PARSER_ERROR_COUNT_LIMIT);
    ASSERT(parser->eh != NULL, "Failed to create Parser error handler.");

    parser->ast = NULL;

    return parser;
}

void del_parser(SParser *parser)
{
    if (parser != NULL) {
        del_scanner(parser->scanner);
        free(parser);
    }
}

const UToken *parserPeekNextNToken(SParser *const parser, const U64 n)
{
    ASSERT(parser, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->tokWind, "Parser tokens window is not initialized!");
    ASSERT_DBG(parser->tokWind->len % 2 == 0, PARSER_INCORRECT_TOKEN_WINDOW_BUFFER_SIZE);
    const U64 tokLookAheadSize = parser->tokWind->len / 2;
    ASSERT(n < tokLookAheadSize,
           "%zu is greater than the token look-ahead size %zu.",
           n,
           tokLookAheadSize);

    const U64 next = parser->tokWind->next;
    const U64 end = parser->tokWind->end;
    const U64 nextNthTokPos = (next + n) % tokLookAheadSize;

    return &(parser->tokWind->toks[nextNthTokPos > end ? end : nextNthTokPos]);
}

const UToken *parserNextToken(SParser *const parser)
{
    ASSERT(parser, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->tokWind, "Parser tokens window is not initialized!");

    const UToken *pNextTok = tokWind_getNext(parser->tokWind);

    nextTok(parser->scanner);
    const UToken *const pNextEndTok = &(parser->scanner->tok);

    // This updates the next index token window, so the next token
    // must be retrieved beforehand!
    tokWind_advance(parser->tokWind, pNextEndTok);

    if (parser->scanner->err != EERROR_TYPE_NO_ERROR) {
        SError err;
        initError(&err, parser->scanner->err, pNextEndTok);
        eh_handle(parser->eh, &err);
    }

    return pNextTok;
}

const UToken *parserPeekPrevNToken(SParser *const parser, const U64 n)
{
    ASSERT(parser, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->tokWind, "Parser tokens window is not initialized!");
    const STokWind *tw = parser->tokWind;
    ASSERT_DBG(tw->len % 2 == 0, PARSER_INCORRECT_TOKEN_WINDOW_BUFFER_SIZE);
    const U64 twLen = tw->len;
    const U64 tokLookBackSize = twLen / 2;
    ASSERT(tokLookBackSize > n,
           "Peeking back previous %zuth token violates the token look-back size "
           "%zu and goes out of bounds of the tokens window buffer.",
           n + 1,
           tokLookBackSize);

    const U64 next = tw->next;
    const U64 end = tw->end;
    U64 peekablePrevTokCount = tokLookBackSize;

    // if # processed tokens <= tokLookBackSize
    if (next < end && end > (twLen - 1))
        peekablePrevTokCount = next;

    const U64 peekOffset = n + 1;
    if (peekablePrevTokCount < peekOffset)
        return NULL;

    const U64 peekPos = (next - peekOffset + twLen) % twLen;

    return &(tw->toks[peekPos]);
}

const UToken *parserPeekToken(SParser *const parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->tokWind != NULL, PARSER_TOKEN_WINDOW_NOT_INITIALIZED);

    const UToken *const toks = parser->tokWind->toks;
    const U64 next = parser->tokWind->next;

    return &(toks[next]);
}

static SStructDecl *parseStructDecl(SParser *const parser, Vec *const declSpecifiers)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

static SEnumDecl *parseEnumDecl(SParser *const parser, Vec *const declSpecifiers)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

static SUnionDecl *parseUnionDecl(SParser *const parser, Vec *const declSpecifiers)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

static SFuncDecl *parseFuncDecl(SParser *const parser, Vec *const declSpecifiers)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

static SVarDecl *parseVarDecl(SParser *const parser, Vec *const declSpecifiers)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

static Vec *parseDeclSpecifiers(SParser *const parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    Vec *const declSpecifiers = new_vecSDeclSpecifier(0, 5);
    for (const UToken *const tok = parserPeekToken(parser);
         tok->base.type != ETOKEN_EOF && eTokenIsDeclSpecifier(tok->base.type);
         tok = parserPeekToken(parser)) {
        SDeclSpecifier declSp;
        initSDeclSpecifier(&declSp, tok);
        vecSDeclSpecifier_push(declSpecifiers, &declSp);
    }

    // Verify that there's AT MOST ONE storage class decl specifier
    U64 strgClassCount = 0;
    for (U64 i = 0; i < declSpecifiers->len; i++) {
        const SDeclSpecifier *const declSp = vecSDeclSpecifier_get(declSpecifiers, i);
        ASSERT_DBG(declSp,
                   "WHAT??? vec_get by index is returning NULL???? "
                   "While going through the vector elements in a FOR LOOP????");
        if (declSp->type == EDECLSPECIFIERTYPE_STORAGE_CLASS)
            strgClassCount++;
    }
    if (strgClassCount > 1) {
        ASSERT_DBG(parser->eh,
                   "Come on, bro! You should've already initiailized "
                   "the parser error handler! WTF?!");
        // TODO: HANDLE THE ERROR
        // TODO: HOW DO YOU PROPOGATE THE STATE OF THE ERROR HANDLING IN CASE
        //       IT HITS THE LIMIT OF ERROR COUNT AND THE COMPILATION PROCESS
        //       NEEDS TO BE TERMINATED SO THAT THE ERRORS ARE OUTPUTTED???
    }

    return declSpecifiers;
}

static SDecl *parseDecl(SParser *const parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");

    U64 peekNextLvl = 0;
    SDeclSpecifier *declSpecifier = NULL;
    Vec *declSpecifiers = parseDeclSpecifiers(parser);
    ASSERT(declSpecifiers, "Failed to create a new vector of SDeclSpecifier.");
    for (const UToken *tok = parserPeekToken(parser); tok->base.type != ETOKEN_EOF;
         tok = parserPeekToken(parser)) {
        const EToken tokType = tok->base.type;
        switch (tokType) {
        case ETOKEN_STRUCT:
            return (SDecl *)parseStructDecl(parser, declSpecifiers);

        case ETOKEN_UNION:
            return (SDecl *)parseUnionDecl(parser, declSpecifiers);

        case ETOKEN_ENUM:
            return (SDecl *)parseEnumDecl(parser, declSpecifiers);

        case ETOKEN_INLINE:
            return (SDecl *)parseFuncDecl(parser, declSpecifiers);

        case ETOKEN_IDENT:

        case ETOKEN_STATIC:
        case ETOKEN_EXTERN:
        case ETOKEN_TYPEDEF:
        case ETOKEN_AUTO:
            UDeclSpecifierCore declSpCore;
            SDeclSpecifier declSp;

            vecSDeclSpecifier_push(declSpecifiers, );
            break;

        case ETOKEN_VOLATILE:

        default:
            if (eTokenIsTypeSpecifier(tokType)) {
            }
            return NULL;
        }
    }

    return NULL;
}

static SExpr *parseExpr(SParser *const parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

static SStmt *parseStmt(SParser *const parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

void parserBuildAST(SParser *const parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->scanner != NULL, "Init the scanner before building a parser AST!");

    const UToken *tok = parserPeekToken(parser);
    if (tok->base.type == ETOKEN_EOF)
        return;

    SParserAST *ast = (SParserAST *)malloc(sizeof(SParserAST));

    while (tok->base.type != ETOKEN_EOF) {
        if (eTokenIsKeyword(tok->base.type)) {
            SDecl *decl = parseDecl(parser);
        } else {
            // TODO: parse other type of tokens here...
        }

        tok = parserPeekToken(parser);
    }
}
