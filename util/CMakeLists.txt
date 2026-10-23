add_library(volt_util
    vlt_files.c
)
target_include_directories(volt_util
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(volt_util
    PUBLIC
    compiler_warnings
    volt_config
)

add_subdirectory(core)
