//
// Created by gomil on 2023/11/20.
//

#ifndef KALEIDOSCOPE_FILE_PARSER_H
#define KALEIDOSCOPE_FILE_PARSER_H

#include "token.h"
#include <memory>
#include <vector>
#include "top_level.h"
#include "cursor.h"
#include "string_reader.h"


class FileParser{
public:
    explicit FileParser(std::string_view view): reader(view){
    }

    ErrorOr<std::unique_ptr<FileAst>> Parse();
    const Token& GetCurrentToken() const noexcept{
        return reader.GetCurrentToken();
    }
private:
    StringReader reader;
    /**
 * Get the next token and save in current
 */
    void Bump();

    /// fn a(a,b)
    ErrorOr<std::unique_ptr<CallableDeclaration>> ParseProtoType();
    ErrorOr<std::unique_ptr<Block>> ParseBody();

};

#endif //KALEIDOSCOPE_FILE_PARSER_H
