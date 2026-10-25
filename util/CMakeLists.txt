add_library(volt_cutil
    vlt_files.c
    vlt_vector.c
    vlt_hashmap_sep_chaining.c
    vlt_string.c
    vlt_stringBuilder.c
)
target_include_directories(volt_cutil
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(volt_cutil
    PUBLIC
    compiler_warnings
    volt_config
    volt_cutil_core
)

add_subdirectory(core)

# TEST
target_link_libraries(execvctest
    PRIVATE
    volt_cutil
)
target_sources(execvctest
    PRIVATE
    vlt_vector_test.c
    vlt_hashmap_test.c
    vlt_string_test.c
    vlt_stringBuilder_test.c
)
