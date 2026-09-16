#include "base_inc.h"
#include "frontend/C/scanner.h"

#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("No file is provided!\n");
        return 0;
    }

    String* filepath = new_stringFromLit(argv[1]);
    ASSERT(filepath != NULL, NULL_POINTER_ERROR_MSG_FORMAT, "String");
    SScanner* scanner = new_scanner(filepath);
    ASSERT(scanner != NULL, FAILED_TO_ALLOC_MEM_FOR_FORMAT, "SScanner object");
    printf("tok = %d\n", scanner->tok.type);

    return 0;
}
