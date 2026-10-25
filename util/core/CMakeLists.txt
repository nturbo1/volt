add_library(volt_cutil_core
    vlt_hash.c
    vlt_bytes.c
)
target_include_directories(volt_cutil_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(volt_cutil_core
    PUBLIC
    compiler_warnings
    volt_config
)
