cmake_minimum_required(VERSION 3.5)

project(Silicium)
include(CTest)

# Set the C++ standard to C++20

set(CMAKE_CXX_STANDARD 20)

# search for packages
FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

# Source files
set(SILICIUM_SOURCES
    AstNode.cpp
    tokens.l
    parser.y
    main.cpp
)

set(SILICIUM_HEADERS
    AstNode.hpp
)

# Set the version number
set(VER_MAJ 0)
set(VER_MIN 1)
set(VER_PATCH 0)

# copy some headers to the build directory
file(COPY ${SILICIUM_HEADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Generate the parser and scanner
BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
FLEX_TARGET(Scanner tokens.l ${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp)

# Build the Compiler
add_executable(sic ${SILICIUM_SOURCES} ${SILICIUM_HEADERS} ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})

add_test(NAME test_silicium COMMAND sic ../test/grammar.si)
add_test(NAME lexer_error COMMAND sic ../test/lex_error.si)
set_property(TEST lexer_error PROPERTY WILL_FAIL TRUE)