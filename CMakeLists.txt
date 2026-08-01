cmake_minimum_required(VERSION 3.20)
project(volt C)
include(cmake/configs.cmake)

add_executable(volt main.c)

add_subdirectory(base)

target_link_libraries(volt
    PRIVATE
    volt_c_base
)
