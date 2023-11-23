#ifndef KALEIDOSCOPE_CURSOR_H
#define KALEIDOSCOPE_CURSOR_H

#include "token.h"
#include <iostream>
#include <iterator>

class Cursor{
public:
    Token AdvanceToken();

    explicit Cursor(std::string_view& chars): chars(chars){

    }
private:
    const std::string_view chars;
    size_t index = 0;
    char Bump();
    Token parseIdent(char prev);
    Token parseNumber(char prev);
    [[nodiscard]] char First () const;
};

#endif //KALEIDOSCOPE_CURSOR_H