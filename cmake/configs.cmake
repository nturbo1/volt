set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(compiler_warnings INTERFACE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(compiler_warnings INTERFACE
        -Wall
        -Wextra
        -Werror
        -Wpedantic
        -Wshadow
    )
else()
    message(FATAL_ERROR "Only the GNU Compiler is supported at the moment!")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set(DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies" CACHE PATH "The dependencies directory")
