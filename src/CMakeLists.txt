cmake_minimum_required(VERSION 3.5)

project(Silicium)
include(CTest)

# Set the C++ standard to C++20

set(CMAKE_CXX_STANDARD 20)

# search for packages
FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

# We incorporate the CMake features provided by LLVM:
if( $ENV{LLVM_ROOT} )
  set(LLVM_ROOT $ENV{LLVM_ROOT})
endif()
find_package(LLVM PATHS "${LLVM_ROOT}/lib/cmake/llvm" "$ENV{ProgramFiles}/lib/cmake/llvm")

if( LLVM_FOUND )
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_DIR}")
else()
    message(FATAL_ERROR "LLVM installation not found. Set LLVM_ROOT to the installation root path.")
endif()

include(LLVMConfig)
message(STATUS "Found LLVM Package Version:${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM Built type : ${LLVM_BUILD_TYPE}")
string(REGEX MATCH "[0-9]+\\.[0-9]+" LLVM_VER ${LLVM_PACKAGE_VERSION} )

# Source files
set(SILICIUM_SOURCES
    tokens.l
    parser.y
    main.cpp
    FunctionDeclaration.cpp
    Globals.cpp
)

set(SILICIUM_HEADERS
    AstNode.hpp
    FunctionDeclaration.hpp
    ErrorCode.h
    Globals.hpp
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

add_test(NAME lexer_error COMMAND sic ../test/lex_error.si)
set_property(TEST lexer_error PROPERTY WILL_FAIL TRUE)

add_test(NAME parser_error COMMAND sic ../test/parse_error.si)
set_property(TEST parser_error PROPERTY WILL_FAIL TRUE)

add_test(NAME basic_grammar COMMAND sic ../test/grammar.si)
