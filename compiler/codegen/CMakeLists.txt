project(codegen)

add_library(codegen
        function_context.cpp
        include/handle.h
        file_context.cpp
)

target_include_directories(codegen INTERFACE include)

target_link_libraries(codegen
        PRIVATE
        lexer
        common
        ast
        ${llvm_libs}
)