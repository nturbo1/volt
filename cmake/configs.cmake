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
set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include" CACHE PATH "The project-wide include directory")

# --------------------------------------------------------------------
# Testing
# --------------------------------------------------------------------
option(BUILD_TESTING "Build unit tests" OFF)

if(BUILD_TESTING)
    # GoogleTest configuration
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    add_subdirectory(
        "${DEPENDENCIES_DIR}/googletest-1.18.0"
        "${CMAKE_CURRENT_BINARY_DIR}/_deps/googletest-build"
        EXCLUDE_FROM_ALL
    )

    add_subdirectory(tests EXCLUDE_FROM_ALL)
endif()
