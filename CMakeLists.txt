cmake_minimum_required(VERSION 3.25)
project(kaleidoscope)
set(CMAKE_CXX_STANDARD 17)

enable_testing()
find_package(GTest CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
add_executable(kaleidoscope main.cpp)
# common
add_library(common
        common/empty.cpp
)
target_include_directories(common INTERFACE common/include)
# common end

# lexer
add_library(lexer
        compiler/lexer/cursor.cpp
)
target_include_directories(lexer INTERFACE compiler/lexer/include)
# lexer end

# ast
add_library(ast
        compiler/ast/ast.cpp
        compiler/ast/ast_parser.cpp
        compiler/ast/declaration.cpp
)
target_include_directories(ast INTERFACE compiler/ast/include)
target_link_libraries(ast PRIVATE lexer common)
# ast end

# tests
add_executable(
        kaleidoscope_test
        tests/lexer_test.cpp
        tests/ast_test.cpp
)
target_link_libraries(
        kaleidoscope_test
        PRIVATE
        GTest::gtest_main
        lexer
        ast
        common
)
include(GoogleTest)
gtest_discover_tests(kaleidoscope_test)
# tests end