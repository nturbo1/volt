# VOLT - Versatile Optimizing Language Toolchain

## TODO
- Add tests for newly added string functions in `base/vlt_string.h`
- Implement C pre-processor
- Implement error recovery mechanism for the C scanner. For instance, skipping characters until (including) a ';' when a lexical error is detected.
- Consider passing in a pointer for the object to a function instead of returning it from a function, so that the return value of the function can be used as the indication of the error state. For instance, `parse` functions should handle parser/lexer errors while processing tokens and should indicate or jump to compilation process termination and outputting accumulated errors if the error count limit is hit.
