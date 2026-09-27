#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "token.h"
#include "base_inc.h"

#include <stdlib.h>
#include <stdbool.h>

#define PARSER_TOKEN_LOOKAHEAD_RING_CAP 5
#define PARSER_ERROR_COUNT_LIMIT 5

// =====================================================================
// ============================== SParser ==============================
// =====================================================================
/*
 * Maintains a fixed-size ring of tokens to be looked 
 * ahead. When the next token is returned to the parser,
 * it advances the head of the ring, scans the nth
 * (n is the fixed size of the ring buffer) token and
 * appends it to the end/tail of the ring.
 *
 * For instance:
 *
 *     - Before returning the next token:
 *
 *              head                      tail
 *               0       1     2    ...    n
 *         | next-tok | tok | tok | ... | tok |
 *
 *
 *     - After returning the next token:
 *
 *           tail     head
 *            0        1       2    ...    n
 *         | tok | next-tok | tok | ... | tok |
 */
struct STokenRing
{
    UToken* toks;
    U64 next;
    U64 end;
    U64 toksBufCap;
};

static bool isTokRingFull(STokenRing* tokRing)
{
    ASSERT(tokRing != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "STokenRing");
    return (tokRing->end == (tokRing->toksBufCap - 1)) || (tokRing->end < tokRing->next);
}

static void addNextTok(STokenRing* tokRing, const UToken* const tok)
{
    ASSERT(tokRing != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "STokenRing");
    ASSERT(tok != NULL, "Next token MUST NOT be NULL!");
    // TODO: You shouldn't add any token when the last token in the ring 
    //       buffer is of type ETOKEN_EOF!
    ASSERT_DBG(tokRing->toks[tokRing->end].base.type != ETOKEN_EOF,
               "You have already reached the end of the file, bro!");

    // if the token ring buffer is full
    if (isTokRingFull(tokRing))
        tokRing->next = ( (tokRing->next + 1) % tokRing->toksBufCap );

    tokRing->end = ( (tokRing->end + 1) % tokRing->toksBufCap );
    copyBytesFromTo((const U8* const) tok, sizeof(UToken), (U8*) (tokRing->toks + tokRing->end));
}

// =====================================================================
// ============================== SParser ==============================
// =====================================================================
static void initParserTokLookAheadRing(SParser* parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->scanner != NULL, "Init the scanner before initializing the parser token look-ahead ring!");

    STokenRing* tokLARing = (STokenRing*) malloc(sizeof(STokenRing));
    ASSERT(tokLARing != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "STokenRing");
    tokLARing->toksBufCap = PARSER_TOKEN_LOOKAHEAD_RING_CAP;
    tokLARing->toks = NULL;
    tokLARing->toks = (UToken*) malloc(sizeof(UToken) * tokLARing->toksBufCap);
    ASSERT(tokLARing->toks != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "UToken array");
    // Nullify the token ring buffer
    for (U64 i = 0; i < tokLARing->toksBufCap; i++)
    {
        tokLARing->toks[i].base.col = 0;
        tokLARing->toks[i].base.ln = 0;
        tokLARing->toks[i].ident.lexeme = NULL;
        tokLARing->toks[i].number.val = 0;
    }
    tokLARing->next = 0;
    tokLARing->end = 0;

    for (U64 i = 0; i < tokLARing->toksBufCap; i++)
    {
        const EToken tokType = nextTok(parser->scanner);
        const UToken* const tok = &(parser->scanner->tok);
        addNextTok(parser->tokLookAheadRing, tok);

        if (parser->scanner->err != EERROR_TYPE_NO_ERROR)
        {
            SError err;
            initError(&err, parser->scanner->err, tok);
            vecSError_push(parser->errs, &err);
        }

        if (tokType == ETOKEN_EOF)
            break;
    }

    parser->tokLookAheadRing = tokLARing;
}

SParser* new_parser(String* filepath)
{
    SScanner* scanner = new_scanner(filepath);
    ASSERT(scanner != NULL, "Failed to create SScanner.");
    SParser* parser = (SParser*) malloc(sizeof(SParser));
    ASSERT(parser != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SParser");
    parser->scanner = scanner;
    initParserTokLookAheadRing(parser);
    parser->errs = new_vecSError(0, PARSER_ERROR_COUNT_LIMIT);
    ASSERT(parser->errs != NULL, "Failed to create Parser errors vector.");

    parser->ast = NULL; // TODO: Build AST!

    return parser;
}

void del_parser(SParser* parser)
{
    if (parser != NULL)
    {
        del_scanner(parser->scanner);
        free(parser);
    }
}

SToken* parserNextToken(SParser* parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

SToken* parserPeekToken(SParser* parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    // TODO: Implement!
    return NULL;
}

// static SDecl* parseDecl(SParser* parser)
// {
//     ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
//     // TODO: Implement!
//     return NULL;
// }
//
// static SExpr* parseExpr(SParser* parser)
// {
//     ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
//     // TODO: Implement!
//     return NULL;
// }
//
// static SStmt* parseStmt(SParser* parser)
// {
//     ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
//     // TODO: Implement!
//     return NULL;
// }

// static SParserAST* buildAST(SParser* parser)
// {
//     ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
//     ASSERT(parser->scanner != NULL, "Init the scanner before building a parser AST!");
//     // TODO: Implement!
//
//     return NULL;
// }
