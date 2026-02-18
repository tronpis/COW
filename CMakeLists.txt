#--------------------------------------------
# COW PROGRAMMING LANGUAGE - CMake Build
#--------------------------------------------
cmake_minimum_required(VERSION 3.14)

# Project version
project(COW
    VERSION 2.0.0
    DESCRIPTION "COW Programming Language - Modern C++ Implementation"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(COW_BUILD_TESTS "Build tests" ON)
option(COW_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(COW_BUILD_COMPILER "Build cow compiler" ON)

# Export compile commands for tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    # Remove problematic warnings
    add_compile_options(-Wno-unused-parameter)
endif()

# Sanitizers
if(COW_ENABLE_SANITIZERS AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files for the VM library
set(VM_SOURCES
    src/instruction.cpp
    src/vm.cpp
    src/parser.cpp
)

# Create static library for the VM
add_library(cowvm STATIC ${VM_SOURCES})
set_target_properties(cowvm PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Main interpreter executable
add_executable(cow src/main.cpp)
target_link_libraries(cow cowvm)
set_target_properties(cow PROPERTIES
    OUTPUT_NAME "cow"
)

# Define version for code
target_compile_definitions(cow PRIVATE
    COW_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    COW_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    COW_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    COW_VERSION_STRING="${PROJECT_VERSION}"
)

# Compiler executable
if(COW_BUILD_COMPILER)
    add_executable(cowcomp src/compiler.cpp)
    target_link_libraries(cowcomp cowvm)
endif()

# Testing
if(COW_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS cow cowvm
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(COW_BUILD_COMPILER)
    install(TARGETS cowcomp RUNTIME DESTINATION bin)
endif()

install(DIRECTORY include/cow DESTINATION include)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cow-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Print configuration
message(STATUS "")
message(STATUS "COW Programming Language v${PROJECT_VERSION}")
message(STATUS "==============================")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build tests: ${COW_BUILD_TESTS}")
message(STATUS "Build compiler: ${COW_BUILD_COMPILER}")
message(STATUS "Sanitizers: ${COW_ENABLE_SANITIZERS}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
