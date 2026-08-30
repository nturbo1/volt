cmake_minimum_required(VERSION 3.20)
project(volt C)
include(cmake/configs.cmake)

add_executable(vcc main.c)

add_subdirectory(vctest)
add_subdirectory(base)

target_link_libraries(vcc
    PRIVATE
    volt_c_base
)
