#ifndef FRONTEND_C_SCANNER_H
#define FRONTEND_C_SCANNER_H

#include "config.h"
#include "diagnostics.h"
#include "token.h"

typedef struct ScanSrc SScanSrc;
typedef struct SScanner {
    SScanSrc *src;
    String *filepath; // src filepath
    U64 lnOffs; // line offset in the src file
    U64 colOffs; // column offset in the src file
    UToken tok;
    EErrorType err; // error happened in the last scan
} SScanner;

// Creates a new scanner.
//
// Copies the `filepath`, doesn't take ownership of it!
SScanner *new_scanner(const String *const filepath);
void del_scanner(SScanner *s);
EToken scannerPopNextToken(SScanner *s);
EToken scannerPeekNextToken(SScanner *s);
U8 nextChar(SScanner *s);
U8 peekChar(SScanner *s);

#define SCANNER_BUFFER_SIZE 8192
struct ScanSrc {
    FILE *file; // src file
    // Contains 2 buffers:  Buffer 0              Buffer 1
    //                     [ | | | ... | | | |   | |   | ... | | |    ]
    //                      0 1 2 ........... n-1 n n+1 ......... 2n-1
    U64 bufEnd; // end of the current buffer being processed
    U64 next; // the next character index in the src buffer
    U8 buf[2 * SCANNER_BUFFER_SIZE]; // the src buffer
};

///////////////////////////////////////////////////////////////////////////////////
// HashMap<String, EToken>
///////////////////////////////////////////////////////////////////////////////////
void printHashMapStringEToken(const HashMap *const hm);
void printKeywordsHashMap(); // for debugging purposes

#endif // FRONTEND_C_SCANNER_H
