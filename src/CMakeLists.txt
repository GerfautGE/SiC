cmake_minimum_required(VERSION 3.5)

project(Silicium)

# search for packages
FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

# Source files
set(SILICIUM_SOURCES
    tokens.l
    parser.y
    main.cpp
)

set(SILICIUM_HEADERS
)

set(VER_MAJ 0)
set(VER_MIN 1)
set(VER_PATCH 0)

BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
FLEX_TARGET(Scanner tokens.l ${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp)

add_executable(sic ${SILICIUM_SOURCES} ${SILICIUM_HEADERS} ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})