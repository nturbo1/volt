# Uncomment this if you want to use ctest
# enable_testing()
add_executable(exectests tests_main.cpp)
target_include_directories(exectests
    PUBLIC
    ${DEPENDENCIES_DIR}/googletest-1.18.0/googletest/include
    ${DEPENDENCIES_DIR}/googletest-1.18.0/googlemock/include
)
target_link_libraries(exectests
    PRIVATE
    GTest::gtest_main
)
