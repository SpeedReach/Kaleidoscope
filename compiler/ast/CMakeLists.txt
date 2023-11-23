project(ast)

add_library(ast
        ast_node.cpp
)

target_include_directories(ast INTERFACE include)

target_link_libraries(ast
        PRIVATE
        lexer
        common
        ${llvm_libs}
)