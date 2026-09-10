add_library(volt_c_base
    base.c
    vlt_string.c
    vlt_vector.c
    vlt_hashmap.c
    vlt_stringBuilder.c
)
target_include_directories(volt_c_base
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(volt_c_base
    PUBLIC
    compiler_warnings
)

# TEST
target_sources(execvctest
    PRIVATE
    vlt_hashmap_test.c
    vlt_vector_test.c
    vlt_string_test.c
    vlt_stringBuilder_test.c
)
target_link_libraries(execvctest
    PRIVATE
    volt_c_base
)
