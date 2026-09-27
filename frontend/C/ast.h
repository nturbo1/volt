#ifndef FRONTEND_C_AST_H
#define FRONTEND_C_AST_H

typedef struct SParserASTNode
{
    UToken tok;
}
SParserASTNode;

typedef struct SParserAST
{
    SParserASTNode* root;
}
SParserAST;

typedef struct SExpr
{
    SParserASTNode node;
}
SExpr;

typedef struct SDecl
{
    SParserASTNode node;
}
SDecl;

typedef struct SStmt
{
    SParserASTNode node;
}
SStmt;

#endif // FRONTEND_C_AST_H
