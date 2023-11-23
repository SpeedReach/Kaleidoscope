
project(lexer)

add_library(lexer
        cursor.cpp
        string_reader.cpp
)

target_include_directories(
        lexer
        INTERFACE
        include
)