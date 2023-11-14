//
// Created by gomil on 2023/11/10.
//
#include "include/cursor.h"


char Cursor::First() const {
    if( currentIndex >= chars.length()){
        return EOF;
    }
    return chars.at(currentIndex);
}

char Cursor::Second() const {
    if(currentIndex + 1 >= chars.length()){
        return EOF;
    }
    return chars.at(currentIndex+1);
}

char Cursor::Bump() {
    if( currentIndex >= chars.length()){
        return EOF;
    }
    char current = chars.at(currentIndex);
    currentIndex ++;
    return current;
}

Token Cursor::AdvanceToken() {
    char firstChar = Bump();
    if(firstChar == EOF){
        return Token{EOFTok{},0};
    }
    switch (firstChar) {
        case ',': {
            return Token{Comma{}, 1};
        }
        case '(': {
            return Token{OpenParen{}, 1};
        }
        case ')': {
            return Token{CloseParen{}, 1};
        }
        case ';':{
            return Token{Semicolon{},1};
        }
        // #comment
        case '#': {
            while (First() != '\n') {
                Bump();
            }
            return AdvanceToken();
        }
        case '\n': {
            while (First() == '\n'){
                Bump();
            }
            return AdvanceToken();
        }
        case ' ': {
            while (First() == ' ') {
                Bump();
            }
            return AdvanceToken();
        }
        case '+':
        case '-':
        case '*':
        case '/': return Token{Arithmetic{static_cast<ArithmeticOperator>(firstChar)}, 1};
        case '>': {
            if(First() == '='){
                Bump();
                return Token{GreaterEqual{}, 2};
            }
            return Token{GreaterThan{}, 1};
        }
        case '<': {
            if(First() == '='){
                Bump();
                return Token{LessEqual{}, 2};
            }
            return Token{LessThan{}, 1};
        }
        case '=': {
            if(First() == '='){
                Bump();
                return Token{EqualEqual{},2};
            }
            return Token{Assign{},1};
        }
        case '!':{
            if(First() == '='){
                Bump();
                return Token{NotEqual{},2};
            }
            else{
                //TODO add "not" support
                return Token{Unknown{},1};
            }
        }
        default: {
            //attempt parse number
            if(isdigit(firstChar)){
                return parseNumber(firstChar);
            }
            else if(isalpha(firstChar)){
                return parseIdent(firstChar);
            }
            else{
                return Token{Unknown{},1};
            }
        }
    }
}

Token Cursor::parseIdent(char prev) {
    std::string id;
    id += prev;
    while (isalnum(First())){
        id += Bump();
    }
    return Token{Identifier{id},id.length()};
}

Token Cursor::parseNumber(char prev){
    bool dot = false;
    std::string numStr;
    numStr += prev;
    char next;
    while(true){
        next = First();
        if(next == '.'){
            if(dot){
                return Token{ Unknown{next}, numStr.length() +1};
            }
            dot = true;
        }
        else if(!isdigit(next)){
            break;
        }
        numStr += next;
        Bump();
    }
    double num = stod(numStr);
    return Token{Number{num},numStr.length()};
}