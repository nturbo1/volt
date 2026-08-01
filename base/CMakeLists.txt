add_library(volt_c_base
    vlt_string.c
)

target_include_directories(volt_c_base
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(volt_c_base
    PUBLIC
    compiler_warnings
)
