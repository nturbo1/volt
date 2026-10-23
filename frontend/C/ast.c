#include "ast.h"
#include "token.h"
#include "base_inc.h"

void initAstNode(SParserASTNode* const node,
                 const UToken* const tok,
                 const EASTNodeType type)
{
    ASSERT(node, NULL_POINTER_ERROR_MSG_FORMAT, "SParserASTNode");
    ASSERT(tok, NULL_POINTER_ERROR_MSG_FORMAT, "UToken");
    *( (UToken*)(node->tok) ) = *tok;
    *( (EASTNodeType*)(node->type) ) = type;
}

void initSStorageClass(SStorageClass* const strgClass, const UToken* const tok)
{
    ASSERT(strgClass, NULL_POINTER_ERROR_MSG_FORMAT, "SStorageClass");
    ASSERT(tok, NULL_POINTER_ERROR_MSG_FORMAT, "UToken");
    initAstNode( &(strgClass->node), tok, EASTNODETYPE_STORAGE_CLASS );
    switch(tok->base.type)
    {
    case ETOKEN_STATIC:
        strgClass->type = ESTRGCLASSTYPE_STATIC;
        break;

    case ETOKEN_EXTERN:
        strgClass->type = ESTRGCLASSTYPE_EXTERN;
        break;

    case ETOKEN_TYPEDEF:
        strgClass->type = ESTRGCLASSTYPE_TYPEDEF;
        break;

    case ETOKEN_AUTO: 
        strgClass->type = ESTRGCLASSTYPE_AUTO;
        break;

    case ETOKEN_REGISTER:
        strgClass->type = ESTRGCLASSTYPE_REGISTER;
        break;

    default:
        ASSERT(false,
               "Expected a storage class token, but received %s.",
               eTokenToCStr(tok->base.type));
    }
}

void initSTypeQual(STypeQual* const typeQual, const UToken* const tok)
{
    ASSERT(typeQual, NULL_POINTER_ERROR_MSG_FORMAT, "STypeQual");
    ASSERT(tok, NULL_POINTER_ERROR_MSG_FORMAT, "UToken");
    initAstNode( &(typeQual->node), tok, EASTNODETYPE_TYPE_QUALIFIER );
    switch(tok->base.type)
    {
    case ETOKEN_CONST:
        typeQual->type = ETYPEQUALTYPE_CONST;
        break;

    case ETOKEN_RESTRICT:
        typeQual->type = ETYPEQUALTYPE_RESTRICT;
        break;

    case ETOKEN_VOLATILE:
        typeQual->type = ETYPEQUALTYPE_VOLATILE;
        break;

    default:
        ASSERT(false,
               "Expected a type qualifier token, but received %s.",
               eTokenToCStr(tok->base.type));
    }
}

void initSTypeSpecifier(STypeSpecifier* const typeSp, const UToken* const tok)
{
    ASSERT(typeSp, NULL_POINTER_ERROR_MSG_FORMAT, "STypeSpecifier");
    ASSERT(tok, NULL_POINTER_ERROR_MSG_FORMAT, "UToken");
    initAstNode( &(typeSp->node), tok, EASTNODETYPE_TYPE_SPECIFIER );

    switch(tok->base.type)
    {
    case ETOKEN_STRUCT:
        typeSp->type = ETYPESPECIFIERTYPE_STRUCT;
        break;

    case ETOKEN_UNION:
        typeSp->type = ETYPESPECIFIERTYPE_UNION;
        break;

    case ETOKEN_ENUM:
        typeSp->type = ETYPESPECIFIERTYPE_ENUM;
        break;

    case ETOKEN_CONST:
        typeSp->type = ETYPESPECIFIERTYPE_CONST;
        break;

    case ETOKEN_VOID:
        typeSp->type = ETYPESPECIFIERTYPE_VOID;
        break;

    case ETOKEN_SIGNED:
        typeSp->type = ETYPESPECIFIERTYPE_SIGNED;
        break;

    case ETOKEN_UNSIGNED:
        typeSp->type = ETYPESPECIFIERTYPE_UNSIGNED;
        break;

    case ETOKEN_CHAR:
        typeSp->type = ETYPESPECIFIERTYPE_CHAR;
        break;

    case ETOKEN_SHORT:
        typeSp->type = ETYPESPECIFIERTYPE_SHORT;
        break;

    case ETOKEN_INT:
        typeSp->type = ETYPESPECIFIERTYPE_INT;
        break;

    case ETOKEN_LONG:
        typeSp->type = ETYPESPECIFIERTYPE_LONG;
        break;

    case ETOKEN_FLOAT:
        typeSp->type = ETYPESPECIFIERTYPE_FLOAT;
        break;

    case ETOKEN_DOUBLE:
        typeSp->type = ETYPESPECIFIERTYPE_DOUBLE;
        break;

    case ETOKEN__BOOL:
        typeSp->type = ETYPESPECIFIERTYPE__BOOL;
        break;

    case ETOKEN__COMPLEX:
        typeSp->type = ETYPESPECIFIERTYPE__COMPLEX;
        break;

    default:
        ASSERT(false,
               "Expected a type specifier token, but received %s.",
               eTokenToCStr(tok->base.type));
    }
}

void initSFuncSpecifier(SFuncSpecifier* const funcSp, const UToken* const tok)
{
    ASSERT(funcSp, NULL_POINTER_ERROR_MSG_FORMAT, "SFuncSpecifier");
    ASSERT(tok, NULL_POINTER_ERROR_MSG_FORMAT, "UToken");
    initAstNode( &(funcSp->node), tok, EASTNODETYPE_FUNC_SPECIFIER );
}

void initSDeclSpecifier(SDeclSpecifier* const declSp, const UToken* const tok)
{
    ASSERT(declSp, NULL_POINTER_ERROR_MSG_FORMAT, "SDeclSpecifier");
    ASSERT(tok, NULL_POINTER_ERROR_MSG_FORMAT, "UToken");
    initAstNode( &(declSp->node), tok, EASTNODETYPE_DECL_SPECIFIER );

    if (eTokenIsStorageClass(tok->base.type))
    {
        declSp->type = EDECLSPECIFIERTYPE_STORAGE_CLASS;
        initSStorageClass(declSp->core.storageClass, tok);
    }
    else if (eTokenIsTypeSpecifier(tok->base.type))
    {
        declSp->type = EDECLSPECIFIERTYPE_TYPE_SPECIFIER;
        initSTypeSpecifier(declSp->core.typeSpecifier, tok);
    }
    else if (eTokenIsTypeQualifier(tok->base.type))
    {
        declSp->type = EDECLSPECIFIERTYPE_TYPE_QUALIFIER;
        initSTypeQual(declSp->core.storageClass, tok);
    }
    else if (eTokenIsFuncSpecifier(tok->base.type))
    {
        declSp->type = EDECLSPECIFIERTYPE_FUNC_SPECIFIER;
        initSFuncSpecifier(declSp->core.storageClass, tok);
    }
    else
    {
        ASSERT(false,
               "Invalid declaration specifier token of type %s.",
               eTokenToCStr(tok->base.type));
    }
}
