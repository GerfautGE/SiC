cmake_minimum_required(VERSION 3.5)

project(Silicium)
include(CTest)

# Set the C++ standard to C++20

set(CMAKE_CXX_STANDARD 20)

# Set compilers
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

# search for packages
FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)
FIND_PACKAGE(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


# Source files
set(SILICIUM_SOURCES
    tokens.l
    parser.y
    main.cpp
    FunctionDeclaration.cpp
    Globals.cpp
    CodeGen.cpp
    EmitObject.cpp
)

set(SILICIUM_HEADERS
    AstNode.hpp
    FunctionDeclaration.hpp
    ErrorCode.h
    Globals.hpp
    CodeGen.hpp
    EmitObject.hpp
    Options.hpp
)

# Set the version number
set(VER_MAJ 1)
set(VER_MIN 0)
set(VER_PATCH 0)

# copy some headers to the build directory
file(COPY ${SILICIUM_HEADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Generate the parser and scanner
BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
FLEX_TARGET(Scanner tokens.l ${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp)


# Build the Compiler
add_executable(sic ${SILICIUM_SOURCES} ${SILICIUM_HEADERS} ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})

llvm_map_components_to_libnames(llvm_libs irreader core support native)
# Link against LLVM libraries
target_link_libraries(sic ${llvm_libs})

add_test(NAME lexer_error COMMAND sic ../test/lex_error.si)
set_property(TEST lexer_error PROPERTY WILL_FAIL TRUE)

add_test(NAME parser_error COMMAND sic ../test/parse_error.si)
set_property(TEST parser_error PROPERTY WILL_FAIL TRUE)

add_test(NAME basic_grammar COMMAND sic ../test/grammar.si)
