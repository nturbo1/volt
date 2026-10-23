add_library(volt_config
    base.c
)
target_include_directories(volt_config
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(volt_config
    PUBLIC
    compiler_warnings
)
