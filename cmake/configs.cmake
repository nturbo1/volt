set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(compiler_warnings INTERFACE)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
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

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
