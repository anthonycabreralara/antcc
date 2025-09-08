cmake_minimum_required(VERSION 3.10)
project(SimpleLexer)

# Set Clang explicitly (optional if you set via cmake .. -DCMAKE_CXX_COMPILER=clang++)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(include)

# Source files
set(SOURCES
    src/Lexer.cpp
    src/main.cpp
)

# Add executable
add_executable(antcc ${SOURCES})

# Enable lots of warnings for Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(antcc PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

# Optional: Debug sanitizers for memory issues (address, undefined behavior)
option(ENABLE_SANITIZERS "Enable Clang sanitizers" ON)
if (ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(antcc PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(antcc PRIVATE -fsanitize=address,undefined)
endif()
