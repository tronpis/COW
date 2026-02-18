# Tests for COW Programming Language

# Fetch Catch2
Include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

set(TEST_SOURCES
    test_main.cpp
    test_parser.cpp
    test_vm.cpp
)

add_executable(cow_tests ${TEST_SOURCES})
target_link_libraries(cow_tests PRIVATE cowvm Catch2::Catch2WithMain)

# Copy examples to build directory for tests
file(COPY ${CMAKE_SOURCE_DIR}/examples DESTINATION ${CMAKE_BINARY_DIR}/tests)

# Add tests
include(Catch)
catch_discover_tests(cow_tests)
