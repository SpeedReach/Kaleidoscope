project(parsers)

add_library(parsers
        ast_parser.cpp
        file_parser.cpp
)

target_include_directories(parsers INTERFACE include)

target_link_libraries(parsers
        PUBLIC
        lexer
        ast
        common
)