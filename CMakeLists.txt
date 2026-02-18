#--------------------------------------------
# COW PROGRAMMING LANGUAGE - CMake Build
#--------------------------------------------
cmake_minimum_required(VERSION 3.14)
project(COW VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files for the VM library
set(VM_SOURCES
    src/instruction.cpp
    src/vm.cpp
    src/parser.cpp
)

# Create static library for the VM (allows embedding as library)
add_library(cowvm STATIC ${VM_SOURCES})

# Main interpreter executable
add_executable(cow src/main.cpp)
target_link_libraries(cow cowvm)

# Compiler executable
add_executable(cowcomp src/compiler.cpp)
target_link_libraries(cowcomp cowvm)

# Set default compiler detection for cowcomp if needed
set_target_properties(cow PROPERTIES
    COMPILE_DEFINITIONS "COW_VERSION=\"${PROJECT_VERSION}\""
)

# Installation
install(TARGETS cow cowcomp cowvm
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/cow DESTINATION include)

# Testing option
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print configuration
message(STATUS "COW Language v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
