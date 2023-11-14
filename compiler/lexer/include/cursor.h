#ifndef KALEIDOSCOPE_CURSOR_H
#define KALEIDOSCOPE_CURSOR_H

#include "token.h"
#include <iostream>
#include <iterator>

class Cursor{
private:
    std::string_view chars;
    size_t currentIndex = 0;
    char Bump();
    Token parseIdent(char prev);
    Token parseNumber(char prev);
public:
    [[nodiscard]] char First () const;
    [[nodiscard]] char Second() const;
    Token AdvanceToken();

    explicit Cursor(std::string_view& chars): chars(chars){

    }
};

#endif //KALEIDOSCOPE_CURSOR_H