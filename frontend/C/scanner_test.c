#include "config.h"
#include "scanner.h"
#include "token.h"
#include "vctest.h"
#include "vlt_files.h"
#include "vlt_string.h"
#include "vlt_stringBuilder.h"

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
    StringBuilder *sb = new_stringBuilder(NULL, NULL);
    VCTEST_ASSERT_TRUE(sb != NULL);
    String *testFilepath = new_stringFromLit(__FILE__, NULL, NULL);
    VCTEST_ASSERT_TRUE(testFilepath != NULL);
    String *testFileDirPath = dirName(testFilepath);
    VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
    sb_appendString(sb, testFileDirPath);
    sb_appendStrLit(sb, "tests/scanner/punctuators.c1");
    String *filepath = sb_toString(sb);
    VCTEST_ASSERT_TRUE(filepath != NULL);

    // WHEN
    SScanner *s = new_scanner(filepath);

    // THEN
    VCTEST_ASSERT_TRUE(s != NULL);
    VCTEST_ASSERT_TRUE(s->src);
    VCTEST_ASSERT_TRUE(s->lnOffs == 0);
    VCTEST_ASSERT_TRUE(s->colOffs == 0);

    // Test the next token has been scanned and stored
    VCTEST_ASSERT_TRUE(s->tokInfo.ln == 1);
    VCTEST_ASSERT_TRUE(s->tokInfo.col == 1);
    VCTEST_ASSERT_TRUE(s->tokInfo.tok == ETOKEN_LPAREN);
    VCTEST_ASSERT_TRUE(s->tokInfo.lexeme == NULL);
    VCTEST_ASSERT_TRUE(scanner_diagnostic(s) == EDiagnosticEnd
                       || scanner_diagnostic(s) == EDiagnosticBeg);

    // CLEAN-UP
    del_scanner(s);
    del_stringBuilder(sb);
    del_string(testFilepath);
    del_string(testFileDirPath);
    del_string(filepath);
}

// TEST(testScanPunctuators,
//      "When scan a sequence of punctuators in a file, then"
//      " nextTok should scan correct tokens")
// {
//     const U64 tokensSize = 13;
//     // Tokens and their order MUST match the tokens and the
//     // order they appear in inside the test file
//     EToken tokens[13] = { ETOKEN_LPAREN, // (
//                           ETOKEN_LBRACK, // [
//                           ETOKEN_LBRACE, // {
//
//                           ETOKEN_RPAREN, // )
//                           ETOKEN_RBRACK, // ]
//                           ETOKEN_RBRACE, // }
//
//                           ETOKEN_COMMA, // ,
//                           ETOKEN_PERIOD, // .
//                           ETOKEN_COLON, // :
//                           ETOKEN_SEMICOLON, // ;
//                           ETOKEN_ELLIPSIS, // ...
//                           ETOKEN_ASTERISK, // *
//                           ETOKEN_EOF };
//
//     // GIVEN
//     StringBuilder *sb = new_stringBuilder(NULL, NULL);
//     VCTEST_ASSERT_TRUE(sb != NULL);
//     String *testFilepath = new_stringFromLit(__FILE__, NULL, NULL);
//     VCTEST_ASSERT_TRUE(testFilepath != NULL);
//     String *testFileDirPath = dirName(testFilepath);
//     VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
//     sb_appendString(sb, testFileDirPath);
//     sb_appendStrLit(sb, "tests/scanner/punctuators.c1");
//     String *filepath = sb_toString(sb);
//     VCTEST_ASSERT_TRUE(filepath != NULL);
//
//     SScanner *s = new_scanner(filepath);
//     VCTEST_ASSERT_TRUE(s);
//     VCTEST_ASSERT_TRUE(s->src);
//
//     STokenInfo tokInfo;
//     U64 tokLn = 1;
//     for (U64 i = 0; i < tokensSize; i++) {
//         // WHEN
//         const STokenInfo *pTokInfo = scanner_peek(s);
//         VCTEST_ASSERT_TRUE(pTokInfo);
//
//         // THEN
//         VCTEST_ASSERT_TRUE(pTokInfo->tok == tokens[i]);
//         VCTEST_ASSERT_TRUE(pTokInfo->ln == tokLn);
//         VCTEST_ASSERT_TRUE(pTokInfo->col == 1);
//         VCTEST_ASSERT_TRUE(pTokInfo->lexeme == NULL);
//         VCTEST_ASSERT_TRUE(scanner_diagnostic(s) == EDiagnosticEnd
//                            || scanner_diagnostic(s) == EDiagnosticBeg);
//         scanner_next(s, &tokInfo);
//         tokLn++;
//     }
//
//     // CLEAN-UP
//     del_scanner(s);
//     del_stringBuilder(sb);
//     del_string(testFilepath);
//     del_string(testFileDirPath);
//     del_string(filepath);
// }
//
// TEST(testScanKeywords,
//      "When scan a sequence of keywords in a file, then"
//      " nextTok should scan correct tokens")
// {
//     const U64 tokensSize = eTokenKeywordEnd - eTokenKeywordBeg;
//     // Tokens and their order MUST match the tokens and the
//     // order they appear in inside the test file
//     EToken tokens[eTokenKeywordEnd - eTokenKeywordBeg]
//         = { ETOKEN_CONST,    ETOKEN_IF,       ETOKEN_ELSE,     ETOKEN_FOR,      ETOKEN_BREAK,
//             ETOKEN_CONTINUE, ETOKEN_GOTO,     ETOKEN_RETURN,   ETOKEN_STRUCT,   ETOKEN_UNION,
//             ETOKEN_ENUM,     ETOKEN_SWITCH,   ETOKEN_CASE,     ETOKEN_DEFAULT,  ETOKEN_WHILE,
//             ETOKEN_DO,       ETOKEN_VOID,     ETOKEN_STATIC,   ETOKEN_EXTERN,   ETOKEN_REGISTER,
//             ETOKEN_SIGNED,   ETOKEN_UNSIGNED, ETOKEN_SIZEOF,   ETOKEN_TYPEDEF,  ETOKEN_VOLATILE,
//             ETOKEN_AUTO,     ETOKEN_INLINE,   ETOKEN_RESTRICT, ETOKEN__COMPLEX,
//             ETOKEN__IMAGINARY, ETOKEN__BOOL,    ETOKEN_CHAR,     ETOKEN_SHORT,    ETOKEN_INT,
//             ETOKEN_LONG, ETOKEN_FLOAT,    ETOKEN_DOUBLE,   ETOKEN_EOF };
//
//     // GIVEN
//     StringBuilder *sb = new_stringBuilder(NULL, NULL);
//     VCTEST_ASSERT_TRUE(sb != NULL);
//     String *testFilepath = new_stringFromLit(__FILE__, NULL, NULL);
//     VCTEST_ASSERT_TRUE(testFilepath != NULL);
//     String *testFileDirPath = dirName(testFilepath);
//     VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
//     sb_appendString(sb, testFileDirPath);
//     sb_appendStrLit(sb, "tests/scanner/keywords.c1");
//     String *filepath = sb_toString(sb);
//     VCTEST_ASSERT_TRUE(filepath != NULL);
//
//     SScanner *s = new_scanner(filepath);
//     VCTEST_ASSERT_TRUE(s);
//     VCTEST_ASSERT_TRUE(s->src);
//
//     U64 tokLn = 1;
//     STokenInfo tokInfo;
//     for (U64 i = 0; i < tokensSize; i++) {
//         // WHEN
//         const STokenInfo *pTokInfo = scanner_peek(s);
//         VCTEST_ASSERT_TRUE(pTokInfo);
//
//         // THEN
//         VCTEST_ASSERT_TRUE(pTokInfo->tok == tokens[i]);
//         VCTEST_ASSERT_TRUE(pTokInfo->ln == tokLn);
//         VCTEST_ASSERT_TRUE(pTokInfo->col == 1);
//         VCTEST_ASSERT_TRUE(pTokInfo->lexeme == NULL);
//         VCTEST_ASSERT_TRUE(scanner_diagnostic(s) == EDiagnosticEnd
//                            || scanner_diagnostic(s) == EDiagnosticBeg);
//         scanner_next(s, &tokInfo);
//         tokLn++;
//     }
//
//     // CLEAN-UP
//     del_scanner(s);
//     del_stringBuilder(sb);
//     del_string(testFilepath);
//     del_string(testFileDirPath);
//     del_string(filepath);
// }
//
// TEST(testScanComments,
//      "When scan a sequence of single-line and multi-line commments"
//      " in a file, then nextTok should scan correct tokens with"
//      " correct comments text")
// {
//     // GIVEN
//     StringBuilder *sb = new_stringBuilder(NULL, NULL);
//     VCTEST_ASSERT_TRUE(sb != NULL);
//     String *testFilepath = new_stringFromLit(__FILE__, NULL, NULL);
//     VCTEST_ASSERT_TRUE(testFilepath != NULL);
//     String *testFileDirPath = dirName(testFilepath);
//     VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
//     sb_appendString(sb, testFileDirPath);
//     sb_appendStrLit(sb, "tests/scanner/comments.c1");
//     String *filepath = sb_toString(sb);
//     VCTEST_ASSERT_TRUE(filepath != NULL);
//
//     SScanner *s = new_scanner(filepath);
//     VCTEST_ASSERT_TRUE(s);
//     VCTEST_ASSERT_TRUE(s->src);
//
//     U64 tokLn = 1;
//     const U64 commentsCount = 6;
//     STokenInfo tokInfo;
//     for (U64 i = 0; i < commentsCount; i++) {
//         // WHEN
//         const STokenInfo *pTokInfo = scanner_peek(s);
//         VCTEST_ASSERT_TRUE(pTokInfo);
//
//         // THEN
//         VCTEST_ASSERT_TRUE(pTokInfo->tok == ETOKEN_COMMENT);
//         // TODO: Fix it later: you need to set up the expected comment tokens info
//         //       that is compatible with the test input file
//         // VCTEST_ASSERT_TRUE(pTokInfo->col == 1);
//         VCTEST_ASSERT_TRUE(pTokInfo->ln == tokLn);
//         VCTEST_ASSERT_TRUE(pTokInfo->lexeme == NULL);
//         VCTEST_ASSERT_TRUE(scanner_diagnostic(s) == EDiagnosticEnd
//                            || scanner_diagnostic(s) == EDiagnosticBeg);
//         scanner_next(s, &tokInfo);
//         tokLn++;
//     }
//
//     // CLEAN-UP
//     del_scanner(s);
//     del_stringBuilder(sb);
//     del_string(testFilepath);
//     del_string(testFileDirPath);
//     del_string(filepath);
// }
//
// TEST(testScanOperators,
//      "When scan a sequence of operators in a file, then"
//      " nextTok should scan correct tokens")
// {
//     const U64 tokensSize = eTokenOperatorEnd - eTokenOperatorBeg;
//     // Tokens and their order MUST match the tokens and the
//     // order they appear in inside the test input file
//     const EToken tokens[eTokenOperatorEnd - eTokenOperatorBeg]
//         = { ETOKEN_ADD,
//             ETOKEN_SUB,
//             ETOKEN_ASTERISK, // Scanner/lexer can't figure out if '*' is a
//                              // multiplication operator or a punctuator since
//                              // it doesn't enough context for it. So, it returns
//                              // it as an asterisk (a punctuator) and leave it for
//                              // the parser to decipher the exact type of the token
//                              // depending on the context.
//             ETOKEN_QUO,
//             ETOKEN_REM,
//             ETOKEN_AND,
//             ETOKEN_OR,
//             ETOKEN_XOR,
//             ETOKEN_SHL,
//             ETOKEN_SHR,
//             ETOKEN_AND_NOT,
//             ETOKEN_ADD_ASSIGN,
//             ETOKEN_SUB_ASSIGN,
//             ETOKEN_MUL_ASSIGN,
//             ETOKEN_QUO_ASSIGN,
//             ETOKEN_REM_ASSIGN,
//             ETOKEN_AND_ASSIGN,
//             ETOKEN_OR_ASSIGN,
//             ETOKEN_XOR_ASSIGN,
//             ETOKEN_SHL_ASSIGN,
//             ETOKEN_SHR_ASSIGN,
//             ETOKEN_AND_NOT_ASSIGN,
//             ETOKEN_LAND,
//             ETOKEN_LOR,
//             ETOKEN_RARROW,
//             ETOKEN_INC,
//             ETOKEN_DEC,
//             ETOKEN_EQL,
//             ETOKEN_LSS,
//             ETOKEN_GTR,
//             ETOKEN_ASSIGN,
//             ETOKEN_NOT,
//             ETOKEN_NEQ,
//             ETOKEN_LEQ,
//             ETOKEN_GEQ,
//
//             ETOKEN_EOF };
//
//     // GIVEN
//     StringBuilder *sb = new_stringBuilder(NULL, NULL);
//     VCTEST_ASSERT_TRUE(sb != NULL);
//     String *testFilepath = new_stringFromLit(__FILE__, NULL, NULL);
//     VCTEST_ASSERT_TRUE(testFilepath != NULL);
//     String *testFileDirPath = dirName(testFilepath);
//     VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
//     sb_appendString(sb, testFileDirPath);
//     sb_appendStrLit(sb, "tests/scanner/operators.c1");
//     String *filepath = sb_toString(sb);
//     VCTEST_ASSERT_TRUE(filepath != NULL);
//
//     SScanner *s = new_scanner(filepath);
//     VCTEST_ASSERT_TRUE(s);
//     VCTEST_ASSERT_TRUE(s->src);
//
//     U64 tokLn = 1;
//     STokenInfo tokInfo;
//     for (U64 i = 0; i < tokensSize; i++) {
//         // WHEN
//         const STokenInfo *pTokInfo = scanner_peek(s);
//         VCTEST_ASSERT_TRUE(pTokInfo);
//
//         // THEN
//         VCTEST_ASSERT_TRUE(pTokInfo->tok == tokens[i]);
//         VCTEST_ASSERT_TRUE(pTokInfo->ln == tokLn);
//         VCTEST_ASSERT_TRUE(pTokInfo->col == 1);
//         VCTEST_ASSERT_TRUE(pTokInfo->lexeme == NULL);
//         VCTEST_ASSERT_TRUE(scanner_diagnostic(s) == EDiagnosticEnd
//                            || scanner_diagnostic(s) == EDiagnosticBeg);
//         scanner_next(s, &tokInfo);
//         tokLn++;
//     }
//
//     // CLEAN-UP
//     del_scanner(s);
//     del_stringBuilder(sb);
//     del_string(testFilepath);
//     del_string(testFileDirPath);
//     del_string(filepath);
// }
//
// TEST(testScanDecimalLiterals,
//      "When scan a sequence of decimal literals in a file, then"
//      " nextTok should scan correct tokens")
// {
//     const U64 tokensSize = 24;
//     // Tokens and their order MUST match the tokens and the
//     // order they appear in inside the test input file
//     const String *intLitStrings[24] = { new_stringFromLit("0", NULL, NULL),
//                                         new_stringFromLit("1", NULL, NULL),
//                                         new_stringFromLit("2", NULL, NULL),
//                                         new_stringFromLit("9", NULL, NULL),
//                                         new_stringFromLit("10", NULL, NULL),
//                                         new_stringFromLit("42", NULL, NULL),
//                                         new_stringFromLit("99", NULL, NULL),
//                                         new_stringFromLit("100", NULL, NULL),
//                                         new_stringFromLit("123", NULL, NULL),
//                                         new_stringFromLit("255", NULL, NULL),
//                                         new_stringFromLit("256", NULL, NULL),
//                                         new_stringFromLit("512", NULL, NULL),
//                                         new_stringFromLit("999", NULL, NULL),
//                                         new_stringFromLit("1000", NULL, NULL),
//                                         new_stringFromLit("12345", NULL, NULL),
//                                         new_stringFromLit("123456", NULL, NULL),
//                                         new_stringFromLit("999999", NULL, NULL),
//                                         new_stringFromLit("1000000", NULL, NULL),
//                                         new_stringFromLit("2147483647", NULL, NULL),
//                                         new_stringFromLit("2147483648", NULL, NULL),
//                                         new_stringFromLit("4294967295", NULL, NULL),
//                                         new_stringFromLit("4294967296", NULL, NULL),
//                                         new_stringFromLit("9223372036854775", NULL, NULL),
//                                         new_stringFromLit("1844674407370955", NULL, NULL) };
//
//     // GIVEN
//     StringBuilder *sb = new_stringBuilder(NULL, NULL);
//     VCTEST_ASSERT_TRUE(sb != NULL);
//     String *testFilepath = new_stringFromLit(__FILE__, NULL, NULL);
//     VCTEST_ASSERT_TRUE(testFilepath != NULL);
//     String *testFileDirPath = dirName(testFilepath);
//     VCTEST_ASSERT_TRUE(testFileDirPath != NULL);
//     sb_appendString(sb, testFileDirPath);
//     sb_appendStrLit(sb, "tests/scanner/numbers/decimals.c1");
//     String *filepath = sb_toString(sb);
//     VCTEST_ASSERT_TRUE(filepath != NULL);
//
//     SScanner *s = new_scanner(filepath);
//     VCTEST_ASSERT_TRUE(s);
//     VCTEST_ASSERT_TRUE(s->src);
//
//     U64 tokLn = 1;
//     STokenInfo tokInfo;
//     const STokenInfo *pTokInfo = NULL;
//     for (U64 i = 0; i < tokensSize; i++) {
//         // WHEN
//         pTokInfo = scanner_peek(s);
//         VCTEST_ASSERT_TRUE(pTokInfo);
//
//         // THEN
//         VCTEST_ASSERT_TRUE(pTokInfo->tok == ETOKEN_INT_LIT);
//         VCTEST_ASSERT_TRUE(pTokInfo->ln == tokLn);
//         VCTEST_ASSERT_TRUE(pTokInfo->col == 1);
//         VCTEST_ASSERT_TRUE(stringEqual(pTokInfo->lexeme, intLitStrings[i]));
//         VCTEST_ASSERT_TRUE(scanner_diagnostic(s) == EDiagnosticEnd
//                            || scanner_diagnostic(s) == EDiagnosticBeg);
//         scanner_next(s, &tokInfo);
//         tokLn++;
//     }
//
//     ///////////////////////////////////////////////////////////////////
//     // Test the EOF token at the end
//     ///////////////////////////////////////////////////////////////////
//     // WHEN
//     pTokInfo = scanner_peek(s);
//     VCTEST_ASSERT_TRUE(pTokInfo);
//
//     // THEN
//     VCTEST_ASSERT_TRUE(pTokInfo->tok == ETOKEN_EOF);
//     VCTEST_ASSERT_TRUE(pTokInfo->col == 1);
//     VCTEST_ASSERT_TRUE(pTokInfo->ln == tokLn);
//     VCTEST_ASSERT_TRUE(pTokInfo->lexeme == NULL);
//     VCTEST_ASSERT_TRUE(scanner_diagnostic(s) == EDiagnosticEnd
//                        || scanner_diagnostic(s) == EDiagnosticBeg);
//
//     // CLEAN-UP
//     del_scanner(s);
//     del_stringBuilder(sb);
//     del_string(testFilepath);
//     del_string(testFileDirPath);
//     del_string(filepath);
// }
