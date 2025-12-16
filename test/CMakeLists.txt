# Find the installed GoogleTest package
find_package(GTest REQUIRED)

# Enable testing
enable_testing()

# Create your test target
add_executable(simulator_test
    test_simulator.cpp
)

# Link against GoogleTest and pthread
target_link_libraries(simulator_test 
PRIVATE 
    ${LIB_ALIAS}
    GTest::gtest
    GTest::gtest_main
    pthread
)
