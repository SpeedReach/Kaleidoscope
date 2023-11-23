//
// Created by gomil on 2023/11/21.
//

#ifndef KALEIDOSCOPE_STRING_READER_H
#define KALEIDOSCOPE_STRING_READER_H

#include "cursor.h"

class StringReader{
public:
    explicit StringReader(std::string_view chars): cursor(chars){
        Forward();
        Forward();
    }
    Token Forward();
    const Token& GetCurrentToken() const noexcept{
        return currentToken;
    }
    const Token& GetNextToken() const noexcept{
        return nextToken;
    }

private:
    Cursor cursor;
    //Usually the token that we are handling
    Token nextToken = DUMMY_TOKEN;
    Token currentToken = DUMMY_TOKEN;
    int readTokens = 0;
};

#endif //KALEIDOSCOPE_STRING_READER_H
