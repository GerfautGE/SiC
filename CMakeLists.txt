cmake_minimum_required(VERSION 3.5)

project(Silicim)

# Setup the Makefile
set(CMAKE_VERBOSE_MAKEFILE ON)

# Setup the cmake directory containing numerous cmake scripts and macros.
set(CMAKE_MODULE_PATH "${Silicium_SOURCE_DIR}/cmake")

add_subdirectory(src)