cmake_minimum_required(VERSION 3.20)

project(volt)

include(cmake/configs.cmake)

add_executable(cc++ cc.cpp)

# Uncomment this if you want to use ctest
# enable_testing()
add_executable(exectests tests_main.cpp)
add_subdirectory(${DEPENDENCIES_DIR}/googletest-1.18.0)
target_include_directories(exectests
    PUBLIC
    ${DEPENDENCIES_DIR}/googletest-1.18.0/googletest/include
    ${DEPENDENCIES_DIR}/googletest-1.18.0/googlemock/include
)
target_link_libraries(
  exectests
  GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(exectests)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
