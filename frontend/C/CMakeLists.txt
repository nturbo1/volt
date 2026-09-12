add_library(c_frontend
    scanner.c
)
target_link_libraries(c_frontend
    PUBLIC
    volt_c_base
    compiler_warnings
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
