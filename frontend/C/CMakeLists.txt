add_library(c_frontend
    scanner.c
    token.c
    diagnostics.c
    parser.c
    ast.c
    compiler.c
)
target_link_libraries(c_frontend
    PUBLIC
    compiler_warnings
    volt_config
    volt_cutil
)

# TEST
target_link_libraries(execvctest
    PRIVATE
    c_frontend
)
target_sources(execvctest
    PRIVATE
    scanner_test.c
)
