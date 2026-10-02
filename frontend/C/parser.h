#ifndef FRONTEND_C_PARSER_H
#define FRONTEND_C_PARSER_H

#include "scanner.h"
#include "ast.h"
#include "token.h"
#include "error.h"

// =====================================================================
// ============================== SParser ==============================
// =====================================================================
typedef struct STokWind STokWind;
typedef struct SParser
{
    SScanner* scanner;
    SParserAST* ast;
    STokWind* tokWind;
    SErrHandler* eh;
}
SParser;

SParser* new_parser(const String* const filepath);
void del_parser(SParser* parser);
const UToken* parserNextToken(SParser* const parser);
const UToken* parserPeekToken(SParser* const parser);
void parserBuildAST(SParser* const parser);

// =====================================================================
// ============================ STokWind ===============================
// =====================================================================
/*
 * Maintains a fixed-size tokens buffer window that allows
 * the parser to look back and ahead at most `n` tokens at
 * any given moment like so:
 *
 * (though we don't keep
 *  track of the start
 *  since it can easily be
 *  derived by
 *  `(end + 1) % 2*n`)
 *       |
 *       |
 *      \|/
 *       .
 *     start                      next                         end
 *       0     1    ...   n - 1    n    n + 1   n + 2   ...   2n - 1
 *    | tok | tok | ... |  tok  | tok |  tok  |  tok  | ... |  tok  |
 *
 * When the next token is returned, it advances the `next`
 * index and updates the `start` and `end` indices as
 * there will be a new token scanned and added to the end
 * of the window.
 *
 * For instance:
 *
 *     start                      next                         end
 *       0     1    ...   n - 1    n    n + 1   n + 2   ...   2n - 1
 *    | tok | tok | ... |  tok  | tok |  tok  |  tok  | ... |  tok  |
 *
 *    After returning the next token:
 *
 *      end               start                      next
 *       0                 1    ...   n - 1    n    n + 1   n + 2   ...   2n - 1
 *    | new-scanned-tok | tok | ... |  tok  | tok |  tok  |  tok  | ... |  tok  |
 *
 *
 * - If the last token in the buffer is of type `ETOKEN_EOF`,
 *   then `tokWind_addNextTok` function doesn't add any token
 *   and return false.
 *
 * Initializing the tokens window:
 * 
 *    Scan the next `n` tokens and append them all in the
 *    order they have been scanned to the window buffer. The
 *    remaining slots of the buffer should all be nullified.
 *
 *       start, next                     end
 *            0         1     2    ...  n - 1   n    n + 2   ...   2n - 1
 *     |             | tok | tok | ... | tok | null | null | ... | null |
 *
 */
struct STokWind
{
    UToken* toks;
    U64 next; // index of the next token
    U64 len; // fixed length of the tokens buffer
    U64 end; // end index of the window
};

/*
 * Increments the `next` index and adds a given token to the
 * `end` of the window.
 *
 * @return `false` if no new token was added to the end of
 *         the window and the `next` index wasn't incremented
 *         because the end of the file has been already
 *         reached meaning the token at the `end` of the
 *         window is of type `ETOKEN_EOF`
 *
 *         `true` otherwise
 */
bool tokWind_advance(STokWind* const tw, const UToken* const tok);

/*
 * @return true if the last token in the window buffer is of
 *         type `ETOKEN_EOF` and returns false otherwise.
 */
inline bool tokWind_isEofReached(const STokWind* const tw)
{
    ASSERT(tw, NULL_POINTER_ERROR_MSG_FORMAT, "STokWind");
    return (tw->toks[tw->end].base.type == ETOKEN_EOF);
}

const UToken* tokWind_getNext(const STokWind* const tw);

#endif // FRONTEND_C_PARSER_H
