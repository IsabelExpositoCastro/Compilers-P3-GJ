# -----------------------------------------------------------------------------
# src/stack_module/CMakeLists.txt
# 
# This file includes each module directory (sub-library) so they can be
# built individually with their own CMakeLists.txt.
# Each module (e.g., module_args, module_2) is compiled as a static library to allow
# individual development, compilation, and unit testing in isolation.
#
# By organizing code this way:
# - Each developer works on a separate module with minimal merge conflicts.
# - Modules can be independently tested with their own test_<module>.c files.
# - A main program (optional) can link all modules together for integration.
#
# This CMake file only declares the libraries and their internal includes.
# The main executable and test targets are defined in the top-level and test
# CMakeLists.txt files respectively.
# -----------------------------------------------------------------------------

# Add utils library from the top-level src folder
add_library(Stack_Module stack.c)
target_include_directories(Stack_Module PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Stack_Module PRIVATE utils)