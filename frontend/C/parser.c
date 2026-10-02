#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "token.h"
#include "base_inc.h"

#include <stdlib.h>
#include <stdbool.h>

#define PARSER_TOKEN_LOOKAHEAD_SIZE 5
#define PARSER_ERROR_COUNT_LIMIT 5

#define PARSER_TOKEN_WINDOW_NOT_INITIALIZED \
    "Parser token look-ahead ring buffer is not initialized!"
#define PARSER_INCORRECT_TOKEN_WINDOW_BUFFER_SIZE \
    "Incorrect parser token window buffer size, the size MUST be an even number."

bool tokWind_advance(STokWind* const tw, const UToken* const tok)
{
    ASSERT(tw != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "STokWind");
    ASSERT(tok != NULL, "Next token MUST NOT be NULL!");
    ASSERT_DBG(tw->toks[tw->end].base.type != ETOKEN_EOF,
               "You have already reached the end of the file, bro!");

    if (tw->toks[tw->end].base.type == ETOKEN_EOF)
        return false;

    tw->end = ( (tw->end + 1) % tw->len );
    copyBytesFromTo((const U8* const) tok,
                    sizeof(UToken),
                    (U8*) (tw->toks + tw->end));
    tw->next++;

    return true;
}

const UToken* tokWind_getNext(const STokWind* const tw)
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
static void initParserTokWind(SParser* parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->scanner != NULL,
           "Init the scanner before initializing the parser token window!");

    STokWind* tokWind = (STokWind*) malloc(sizeof(STokWind));
    ASSERT(tokWind != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "STokWind");
    tokWind->len = (2 * PARSER_TOKEN_LOOKAHEAD_SIZE);
    tokWind->toks = NULL;
    tokWind->toks = (UToken*) malloc(sizeof(UToken) * tokWind->len);
    ASSERT(tokWind->toks != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "UToken array");

    // Nullify the tokens window buffer
    for (U64 i = 0; i < tokWind->len; i++)
    {
        tokWind->toks[i].base.col = 0;
        tokWind->toks[i].base.ln = 0;
        tokWind->toks[i].ident.lexeme = NULL;
        tokWind->toks[i].number.val = 0;
    }
    tokWind->end = 0;
    tokWind->next = 0;

    ASSERT_DBG(tokWind->len % 2 == 0, PARSER_INCORRECT_TOKEN_WINDOW_BUFFER_SIZE);
    const U64 tokLookAheadSize = tokWind->len / 2;
    for (U64 i = 0; i < tokLookAheadSize; i++)
    {
        nextTok(parser->scanner);
        const UToken* const tok = &(parser->scanner->tok);
        const bool isEof = tokWind_advance(parser->tokWind, tok);

        if (parser->scanner->err != EERROR_TYPE_NO_ERROR)
        {
            SError err;
            initError(&err, parser->scanner->err, tok);
            eh_handle(parser->eh, &err);
        }

        if (isEof)
            break;
    }

    parser->tokWind = tokWind;
}

SParser* new_parser(const String* const filepath)
{
    SScanner* scanner = new_scanner(filepath);
    ASSERT(scanner != NULL, "Failed to create SScanner.");
    SParser* parser = (SParser*) malloc(sizeof(SParser));
    ASSERT(parser != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SParser");
    parser->scanner = scanner;

    initParserTokWind(parser);

    parser->eh = new_errHandler(PARSER_ERROR_COUNT_LIMIT);
    ASSERT(parser->eh != NULL, "Failed to create Parser error handler.");

    parser->ast = NULL;

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

const UToken* parserNextToken(SParser* const parser)
{
    ASSERT(parser, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->tokWind, "Parser tokens window is not initialized!");

    const UToken* pNextTok = tokWind_getNext(parser->tokWind);

    nextTok(parser->scanner);
    const UToken* const pNextEndTok = &(parser->scanner->tok);

    // This updates the next index token window, so the next token
    // must be retrieved beforehand!
    tokWind_advance(parser->tokWind, pNextEndTok);

    if (parser->scanner->err != EERROR_TYPE_NO_ERROR)
    {
        SError err;
        initError(&err, parser->scanner->err, pNextEndTok);
        eh_handle(parser->eh, &err);
    }

    return pNextTok;
}

const UToken* parserPeekToken(SParser* const parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->tokWind != NULL, PARSER_TOKEN_WINDOW_NOT_INITIALIZED);

    const UToken* const toks = parser->tokWind->toks;
    const U64 next = parser->tokWind->next;

    return &(toks[next]);
}

// static SDecl* parseDecl(SParser* const parser)
// {
//     ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
//     // TODO: Implement!
//     return NULL;
// }
//
// static SExpr* parseExpr(SParser* const parser)
// {
//     ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
//     // TODO: Implement!
//     return NULL;
// }
//
// static SStmt* parseStmt(SParser* const parser)
// {
//     ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
//     // TODO: Implement!
//     return NULL;
// }

void parserBuildAST(SParser* const parser)
{
    ASSERT(parser != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "SParser");
    ASSERT(parser->scanner != NULL, "Init the scanner before building a parser AST!");
    
    // const UToken* tok = parserPeekToken(parser);
    // if (eTokenIsKeyword(tok->base.type))
    // {
    // }
    // switch(tok->base.type)
    // {
    // case ETOKEN_INVALID_IDENT:
    // }

}
