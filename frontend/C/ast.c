#include "ast.h"
#include "token.h"
#include "base_inc.h"

void initAstNode(SParserASTNode* const node, const UToken* const tok)
{
    ASSERT(node, NULL_POINTER_ERROR_MSG_FORMAT, "SParserASTNode");
    node->tok = *tok;
}
