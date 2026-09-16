#include "scanner.h"
#include "vctest.h"
#include "base_inc.h"

#include <stdio.h>

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
    String* filepath = sb_ToString(sb);
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
    VCTEST_ASSERT_TRUE(s->tok.ln == 0);
    VCTEST_ASSERT_TRUE(s->tok.col == 0);
    VCTEST_ASSERT_TRUE(s->tok.type == ETOKEN_NO_VALUE);
    VCTEST_ASSERT_TRUE(s->tok.lexeme == NULL);

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
    String* filepath = sb_ToString(sb);
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
    VCTEST_ASSERT_TRUE(s->tok.ln == 0);
    VCTEST_ASSERT_TRUE(s->tok.col == 0);
    VCTEST_ASSERT_TRUE(s->tok.type == ETOKEN_NO_VALUE);
    VCTEST_ASSERT_TRUE(s->tok.lexeme == NULL);

    for (U64 i = 0; i < tokensSize; i++)
    {
        nextTok(s);
        VCTEST_ASSERT_TRUE(s->tok.type == tokens[i]);
        VCTEST_ASSERT_TRUE(s->tok.lexeme == NULL);
    }

    // CLEAN-UP
    del_scanner(s);
    del_stringBuilder(sb);
    del_string(testFilepath);
    del_string(testFileDirPath);
    del_string(filepath);
}
