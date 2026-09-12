#ifndef FRONTEND_C_SCANNER_H
#define FRONTEND_C_SCANNER_H

#include "base_inc.h"
#include "token.h"

#define SCANNER_BUFFER_SIZE 8192

typedef struct SScanner
{
    FILE* file;
    String* filepath;

    // Contains 2 buffers:  Buffer 0              Buffer 1
    //                     [ | | | ... | | | |   | |   | ... | | |    ]
    //                      0 1 2 ........... n-1 n n+1 ......... 2n-1
    U64 bufEnd;     // end of the current buffer being processed
    U64 nextChIdx;  // the next character index in the src buffer
    U64 lnOffs;     // line offset in the src file
    U64 colOffs;    // column offset in the src file
    EToken tok;
    String* tokLexeme;
    U64 tokLn;
    U64 tokCol;
    U8 buf[2 * SCANNER_BUFFER_SIZE]; // the src buffer
}
SScanner;

SScanner* new_scanner(String* filepath);
void del_scanner(SScanner* s);
EToken nextTok(SScanner* s);
EToken peekTok(SScanner* s);
U8 next(SScanner* s);
U8 peek(SScanner* s);

#endif // FRONTEND_C_SCANNER_H
