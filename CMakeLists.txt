cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_EXTENSIONS OFF)

project(
    grace
    VERSION 0.1
    LANGUAGES CXX
)

# Generate a .gitignore in the build directory to ignore all its contents
if (PROJECT_IS_TOP_LEVEL AND NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    file(WRITE "${CMAKE_BINARY_DIR}/.gitignore" "*\n")
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_subdirectory(lib)

option(grace_ENABLE_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})

if (grace_ENABLE_TESTS)
    add_subdirectory(test)
endif()
