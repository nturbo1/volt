#include "base_inc.h"

#include <stdio.h>

int main()
{
    String* s1 = new_string_from_lit("A new String 1!");
    printf("s1 = %s\t&s1->bytes = %p\n", s1->bytes, (void*) s1->bytes);
    String* s2 = new_string((const U8* const) "A new String 2!", 15);
    printf("s2 = %s\t&s2->bytes = %p\n", s2->bytes, (void*) s2->bytes);
    del_string(s1);
    s1 = NULL;
    del_string(s2);
    s2 = NULL;

    return 0;
}
