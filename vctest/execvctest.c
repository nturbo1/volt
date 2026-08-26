#include "vctest.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        vctest_run_all(false);
    }
    else if (argc == 2)
    {
        const char* param = argv[1];
        size_t len = strlen(argv[1]);

        if ((len == 2 && param[1] == 'v') ||
            (len > 2 && strncmp(param, "--verbose", len) == 0))
        {
            vctest_run_all(true);
        }
        else
        {
            PRINT_HELP();
        }
    }
    else
    {
        PRINT_HELP();
    }

    return 0;
}
