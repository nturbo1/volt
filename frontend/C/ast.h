#ifndef FRONTEND_C_AST_H
#define FRONTEND_C_AST_H

typedef enum EASTNodeType
{
    EASTNODETYPE_DECL,
    EASTNODETYPE_EXPR,
    EASTNODETYPE_STMT,
}
EASTNodeType;

typedef struct SParserASTNode
{
    UToken tok;
    EASTNodeType type;    
}
SParserASTNode;

void initAstNode(SParserASTNode* const node, const UToken* const tok);

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


////////////////////////////////////////////////////////////////////////////////
//////// Declarations
////////////////////////////////////////////////////////////////////////////////
typedef enum ETypeQualType
{
    ETYPEQUALTYPE_CONST,
    ETYPEQUALTYPE_RESTRICT,
    ETYPEQUALTYPE_VOLATILE,
    ETYPEQUALTYPE_NONE
}
ETypeQualType;
typedef struct STypeQual
{
    const SParserASTNode node;
    ETypeQualType type;
}
STypeQual;

typedef enum ETypeSpecifierType
{
    ETYPESPECIFIERTYPE_VOID,
    ETYPESPECIFIERTYPE_CHAR,
    ETYPESPECIFIERTYPE_SHORT,
    ETYPESPECIFIERTYPE_INT,
    ETYPESPECIFIERTYPE_LONG,
    ETYPESPECIFIERTYPE_FLOAT,
    ETYPESPECIFIERTYPE_DOUBLE,
    ETYPESPECIFIERTYPE_SIGNED,
    ETYPESPECIFIERTYPE_UNSIGNED,
    ETYPESPECIFIERTYPE__BOOL,
    ETYPESPECIFIERTYPE__COMPLEX,
    ETYPESPECIFIERTYPE_STRUCT,
    ETYPESPECIFIERTYPE_UNION,
    ETYPESPECIFIERTYPE_ENUM,
    ETYPESPECIFIERTYPE_IDENT
}
ETypeSpecifierType;
typedef struct STypeSpecifier
{
    const SParserASTNode node;
    const ETypeSpecifierType type;
}
STypeSpecifier;

typedef struct SFuncSpecifier
{
    const SParserASTNode node;
}
SFuncSpecifier;

typedef enum EStorageClassType
{
    ESTRGCLASSTYPE_TYPEDEF,
    ESTRGCLASSTYPE_EXTERN,
    ESTRGCLASSTYPE_STATIC,
    ESTRGCLASSTYPE_AUTO,
    ESTRGCLASSTYPE_REGISTER,
    ESTRGCLASSTYPE_NONE
}
EStorageClassType;
typedef struct SStorageClass
{
    const SParserASTNode node;
    EStorageClassType type;
}
SStorageClass;

typedef enum EDeclSpecifierType
{
    EDECLSPECIFIERTYPE_STORAGE_CLASS,
    EDECLSPECIFIERTYPE_TYPE_SPECIFIER,
    EDECLSPECIFIERTYPE_TYPE_QUALIFIER,
    EDECLSPECIFIERTYPE_FUNC_SPECIFIER,
}
EDeclSpecifierType;
typedef union UDeclSpecifierCore
{
    SStorageClass storageClass;
    STypeSpecifier typeSpecifier;
    STypeQual typeQual;
    SFuncSpecifier funcSpecifier;
}
UDeclSpecifierCore;

typedef struct SDeclSpecifier SDeclSpecifier;
struct SDeclSpecifier {
    const SParserASTNode node;
    const EDeclSpecifierType type;
    const SDeclSpecifier* next;
    UDeclSpecifierCore* core;
};

typedef enum EDeclType
{
    EDECLTYPE_FUNC,
    EDECLTYPE_VAR,
    EDECLTYPE_STRUCT,
    EDECLTYPE_UNION,
    EDECLTYPE_ENUM,
}
EDeclType;

typedef struct SPointer SPointer;
struct SPointer {
    const SParserASTNode node;
    // Vec<ETypeQual>
    Vec* typeQuals;
    const SPointer* next;
};

typedef enum EDirectDeclaratorType
{
    EDIRECTDECLARATORTYPE_IDENT,
    EDIRECTDECLARATORTYPE_ARRAY,
    EDIRECTDECLARATORTYPE_FUNC,
    EDIRECTDECLARATORTYPE_DECLARATOR,
}
EDirectDeclaratorType;

typedef struct SArrayDirectDeclarator
{
    const SParserASTNode node;
    Vec* typeQuals;
}
SArrayDirectDeclarator;

typedef enum EFunrParamType
{
    EFUNCPARAMTYPE_IDENT,
    EFUNCPARAMTYPE_PARAMTYPE
}
EFuncParamType;

typedef struct SDeclarator SDeclarator;
typedef struct SFuncParamDecl
{
    const SParserASTNode node;
    SDeclSpecifier* declSpecifier;
    SDeclarator* declarator;
}
SFuncParamDecl;

typedef union UFuncParamCore
{
    String ident;
    SFuncParamDecl paramDecl;
}
UFuncParamCore;

typedef union SFuncParam
{
    const SParserASTNode node;
    EFuncParamType type;
    UFuncParamCore* core;
}
SFuncParam;

typedef struct SFuncDirectDeclarator
{
    const SParserASTNode node;
    // Vec<SFuncParam>
    Vec* params;
}
SFuncDirectDeclarator;

typedef union UDirectDeclaratorCore
{
    String* ident;
    SArrayDirectDeclarator array;
    SFuncDirectDeclarator func;
}
UDirectDeclaratorCore;

typedef struct SDirectDeclarator
{
    const SParserASTNode node;
    EDirectDeclaratorType type;
    UDirectDeclaratorCore core;
}
SDirectDeclarator;

struct SDeclarator
{
    const SParserASTNode node;
    SPointer* pointer;
    SDirectDeclarator* directDeclarator;
};

typedef struct SInitializer
{
    const SParserASTNode node;
}
SInitializer;

typedef struct SInitDeclarator
{
    const SParserASTNode node;
    SDeclarator* declarator;
    SInitializer* initializer; // optional
}
SInitDeclarator;

typedef struct SDecl
{
    const SParserASTNode node;
    // Vec<SDeclSpecifier>
    Vec* declSpecifiers;
    // Vec<SInitDeclarator>
    Vec* initDeclaratorList; // optional
    const EDeclType declType; // For deciphering the underlying obj type
                              // when `SDecl *` is used.
}
SDecl;

typedef struct SStructDecl
{
    SDecl decl;
    // TODO: fields container
}
SStructDecl;

typedef struct SEnumDecl
{
    SDecl decl;
    // TODO: enum members container
}
SEnumDecl;

typedef struct SUnionDecl
{
    SDecl decl;
    // TODO: union fields/members container
}
SUnionDecl;

typedef struct SFuncDecl
{
    SDecl decl;
    // TODO: parameters container
    // TODO: func body
    // TODO: return type
    // TODO: identifier/name
}
SFuncDecl;

typedef struct SVarDecl
{
    SDecl decl;
    // TODO: data type
    // TODO: identifier/name
    // TODO: value
}
SVarDecl;


////////////////////////////////////////////////////////////////////////////////
//////// Statements
////////////////////////////////////////////////////////////////////////////////
typedef struct SStmt
{
    SParserASTNode node;
}
SStmt;

#endif // FRONTEND_C_AST_H
