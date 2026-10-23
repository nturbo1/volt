add_library(volt_core_util
    vlt_string.c
    vlt_vector.c
    vlt_hashmap.c
    vlt_stringBuilder.c
    vlt_hash.c
    vlt_bytes.c
)
target_include_directories(volt_core_util
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(volt_core_util
    PUBLIC
    compiler_warnings
    volt_config
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
    volt_core_util
)
