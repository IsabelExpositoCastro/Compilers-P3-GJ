# -----------------------------------------------------------------------------
# tests/CMakeLists.txt
#
# This file configures unit tests for each module using CTest. Each test is
# built as a standalone executable and linked with the corresponding module,
# allowing for isolated unit testing and debugging.
# -----------------------------------------------------------------------------
 
message(STATUS "(${PROJECT_NAME}) Configuring test executables for modules...")

add_executable(test_parser_integration test_parser_integration.c)
target_link_libraries(test_parser_integration PRIVATE utils Automata_Module InArgs_Module Language_Module Parser_Module Stack_Module TokenReader_Module)
target_include_directories(test_parser_integration PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(test_parser_integration PRIVATE PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
add_test(NAME TestParserIntegration COMMAND test_parser_integration)
message(STATUS " - (${PROJECT_NAME}) Integration test for parser pipeline added")

message(STATUS " - (${PROJECT_NAME}) Test configuration (executables) completed.")
