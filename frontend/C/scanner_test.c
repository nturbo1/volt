#include "scanner.h"
#include "vctest.h"
#include "base_inc.h"

static 

// typedef struct TokenLexemePair
// {
//     EToken tok;
//     String* lexeme;
// }
// TokenLexemePair;
//
// static const TokenLexemePair[] = {
//
// }

TEST(testNewScanner,
     "When new_scanner, then return a pointer to a SScanner obj"
     " with properly initialized fields and correctly scanned"
     " first/next token")
{
    // GIVEN
    SStringBuilder* sb = new_stringBuilder();
    VCTEST_ASSERT_TRUE(sb != NULL);
    String* testFilepath = new_stringFromLit(__FILE__);
    VCTEST_ASSERT_TRUE(testFilepath != NULL);
    String* testFileDirPath = dirName(testFilepath);
    VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
    sb_appendString(sb, testFileDirPath);
    sb_appendStrLit(sb, "tests/scanner/punctuators.c1");
    String* filepath = sb_toString(sb);
    VCTEST_ASSERT_TRUE(filepath != NULL);

    // WHEN
    SScanner* s = new_scanner(filepath);

    // THEN
    VCTEST_ASSERT_TRUE(s != NULL);
    VCTEST_ASSERT_TRUE(s->src->file != NULL);
    VCTEST_ASSERT_TRUE(stringEqual(s->filepath, filepath));
    VCTEST_ASSERT_TRUE(s->src->bufEnd > 0);

    // Test no token has been scanned yet
    VCTEST_ASSERT_TRUE(s->src->next == 0);
    VCTEST_ASSERT_TRUE(s->lnOffs == 0);
    VCTEST_ASSERT_TRUE(s->colOffs == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.ln == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.col == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.type == ETOKEN_NO_VALUE);

    // CLEAN-UP
    del_scanner(s);
    del_stringBuilder(sb);
    del_string(testFilepath);
    del_string(testFileDirPath);
    del_string(filepath);
}

TEST(testScanPunctuators,
     "When scan a sequence of punctuators in a file, then"
     " nextTok should scan correct tokens")
{
    const U64 tokensSize = 13;
    // Tokens and their order MUST match the tokens and the
    // order they appear in inside the test file
    EToken tokens[13] = {
        ETOKEN_LPAREN, // (
        ETOKEN_LBRACK, // [
        ETOKEN_LBRACE, // {

        ETOKEN_RPAREN, // )
        ETOKEN_RBRACK, // ]
        ETOKEN_RBRACE, // }

        ETOKEN_COMMA, // ,
        ETOKEN_PERIOD, // .
        ETOKEN_COLON, // :
        ETOKEN_SEMICOLON, // ;
        ETOKEN_ELLIPSIS, // ...
        ETOKEN_ASTERISK, // *
        ETOKEN_EOF
    };

    // GIVEN
    SStringBuilder* sb = new_stringBuilder();
    VCTEST_ASSERT_TRUE(sb != NULL);
    String* testFilepath = new_stringFromLit(__FILE__);
    VCTEST_ASSERT_TRUE(testFilepath != NULL);
    String* testFileDirPath = dirName(testFilepath);
    VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
    sb_appendString(sb, testFileDirPath);
    sb_appendStrLit(sb, "tests/scanner/punctuators.c1");
    String* filepath = sb_toString(sb);
    VCTEST_ASSERT_TRUE(filepath != NULL);

    // WHEN
    SScanner* s = new_scanner(filepath);

    // THEN
    VCTEST_ASSERT_TRUE(s != NULL);
    VCTEST_ASSERT_TRUE(s->src->file != NULL);
    VCTEST_ASSERT_TRUE(stringEqual(s->filepath, filepath));
    VCTEST_ASSERT_TRUE(s->src->bufEnd > 0);

    // Test no token has been scanned yet
    VCTEST_ASSERT_TRUE(s->src->next == 0);
    VCTEST_ASSERT_TRUE(s->lnOffs == 0);
    VCTEST_ASSERT_TRUE(s->colOffs == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.ln == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.col == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.type == ETOKEN_NO_VALUE);

    for (U64 i = 0; i < tokensSize; i++)
    {
        nextTok(s);
        VCTEST_ASSERT_TRUE(s->tok.base.type == tokens[i]);
        VCTEST_ASSERT_TRUE(s->tok.ident.lexeme == NULL);
    }

    // CLEAN-UP
    del_scanner(s);
    del_stringBuilder(sb);
    del_string(testFilepath);
    del_string(testFileDirPath);
    del_string(filepath);
}

TEST(testScanKeywords,
     "When scan a sequence of keywords in a file, then"
     " nextTok should scan correct tokens")
{
    const U64 tokensSize = eTokenKeywordEnd - eTokenKeywordBeg;
    // Tokens and their order MUST match the tokens and the
    // order they appear in inside the test file
    EToken tokens[eTokenKeywordEnd - eTokenKeywordBeg] = {
        ETOKEN_CONST,
        ETOKEN_IF,
        ETOKEN_ELSE,
        ETOKEN_FOR,
        ETOKEN_BREAK,
        ETOKEN_CONTINUE,
        ETOKEN_GOTO,
        ETOKEN_RETURN,
        ETOKEN_STRUCT,
        ETOKEN_UNION,
        ETOKEN_ENUM,
        ETOKEN_SWITCH,
        ETOKEN_CASE,
        ETOKEN_DEFAULT,
        ETOKEN_WHILE,
        ETOKEN_DO,
        ETOKEN_VOID,
        ETOKEN_STATIC,
        ETOKEN_EXTERN,
        ETOKEN_REGISTER,
        ETOKEN_SIGNED,
        ETOKEN_UNSIGNED,
        ETOKEN_SIZEOF,
        ETOKEN_TYPEDEF,
        ETOKEN_VOLATILE,
        ETOKEN_AUTO, 
        ETOKEN_INLINE,
        ETOKEN_RESTRICT,
        ETOKEN__COMPLEX,
        ETOKEN__IMAGINARY,
        ETOKEN__BOOL,
        ETOKEN_CHAR,
        ETOKEN_SHORT,
        ETOKEN_INT,
        ETOKEN_LONG,
        ETOKEN_FLOAT,
        ETOKEN_DOUBLE,
        ETOKEN_EOF
    };

    // GIVEN
    SStringBuilder* sb = new_stringBuilder();
    VCTEST_ASSERT_TRUE(sb != NULL);
    String* testFilepath = new_stringFromLit(__FILE__);
    VCTEST_ASSERT_TRUE(testFilepath != NULL);
    String* testFileDirPath = dirName(testFilepath);
    VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
    sb_appendString(sb, testFileDirPath);
    sb_appendStrLit(sb, "tests/scanner/keywords.c1");
    String* filepath = sb_toString(sb);
    VCTEST_ASSERT_TRUE(filepath != NULL);

    // WHEN
    SScanner* s = new_scanner(filepath);

    // THEN
    VCTEST_ASSERT_TRUE(s != NULL);
    VCTEST_ASSERT_TRUE(s->src->file != NULL);
    VCTEST_ASSERT_TRUE(stringEqual(s->filepath, filepath));
    VCTEST_ASSERT_TRUE(s->src->bufEnd > 0);

    // Test no token has been scanned yet
    VCTEST_ASSERT_TRUE(s->src->next == 0);
    VCTEST_ASSERT_TRUE(s->lnOffs == 0);
    VCTEST_ASSERT_TRUE(s->colOffs == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.ln == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.col == 0);
    VCTEST_ASSERT_TRUE(s->tok.base.type == ETOKEN_NO_VALUE);

    U64 tokLn = 1;
    for (U64 i = 0; i < tokensSize; i++)
    {
        EToken tok = nextTok(s);
        VCTEST_ASSERT_TRUE(s->tok.base.type == tok);
        VCTEST_ASSERT_TRUE(s->tok.base.type == tokens[i]);
        VCTEST_ASSERT_TRUE(s->tok.base.col == 1);
        VCTEST_ASSERT_TRUE(s->tok.base.ln == tokLn);
        tokLn++;
    }

    // CLEAN-UP
    del_scanner(s);
    del_stringBuilder(sb);
    del_string(testFilepath);
    del_string(testFileDirPath);
    del_string(filepath);
}
