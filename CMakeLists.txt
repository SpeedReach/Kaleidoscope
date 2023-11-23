cmake_minimum_required(VERSION 3.25)
project(kaleidoscope)
set(CMAKE_CXX_STANDARD 17)

enable_testing()
find_package(GTest CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(LLVM CONFIG REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(kaleidoscope main.cpp)

# common
add_library(common
        common/empty.cpp
)
target_include_directories(common INTERFACE common/include)
# common end

# lexer
add_subdirectory(compiler/lexer)
# lexer end


# ast
add_subdirectory(compiler/ast)
# ast end


# parser
add_subdirectory(compiler/parsers)
# parser end

add_subdirectory(compiler/codegen)

# tests
add_executable(
        kaleidoscope_test
        tests/lexer_test.cpp
        tests/ast_test.cpp
        tests/codegen_test.cpp
)
target_link_libraries(
        kaleidoscope_test
        PRIVATE
        GTest::gtest_main
        lexer
        ast
        common
        parsers
        codegen
)
include(GoogleTest)
gtest_discover_tests(kaleidoscope_test)
# tests end