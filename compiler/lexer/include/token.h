//
// Created by gomil on 2023/11/9.
//

#ifndef KALEIDOSCOPE_TOKEN_H
#define KALEIDOSCOPE_TOKEN_H


#include <string>
#include <iostream>
#include <variant>
#include "arith_op.h"

// '('
struct OpenParen{};
// ')'
struct CloseParen{};
// ';'
struct Semicolon{};
// '{'
struct OpenBracket{};
// '}'
struct CloseBracket{};
// ','
struct Comma{};

struct Identifier{
    std::string name;
};
// "="
struct Assign{};
// "*"
struct Arithmetic{
    ArithmeticOperator op;
};
// '>'
struct GreaterThan{};
// ">="
struct GreaterEqual{};
// "<="
struct LessThan{};
// "<"
struct LessEqual{};
// "=="
struct EqualEqual{};
// "!="
struct NotEqual{};
// "&&"
struct AndAnd{};
// "||"
struct OrOr{};

// 20.0
struct Number{
    double d;
};
// Unexpected
struct Unknown{
    char c;
};

// '/0'
struct EOFTok{};

using TokenKind = std::variant<
        OpenParen,
        CloseParen,
        Semicolon,
        Assign,
        Arithmetic,
        GreaterThan,
        GreaterEqual,
        LessThan,
        LessEqual,
        EqualEqual,
        NotEqual,
        AndAnd,
        OrOr,
        OpenBracket,
        CloseBracket,
        Comma,
        Identifier,
        Number,
        Unknown,
        EOFTok
        >;

struct Token{
    TokenKind kind = EOFTok{};
    size_t len = 0;
};

const Token DUMMY_TOKEN = Token{EOFTok{},0};

static std::string ToString(const Token& token){
    switch (token.kind.index()) {
        case 0: return "(";
        case 1: return ")";
        case 2: return ";";
        case 3: return "=";
        case 4: {
            auto op = std::get<Arithmetic>(token.kind).op;
            switch (op) {
                case ArithmeticOperator::add: return "+";
                case ArithmeticOperator::subtract: return "-";
                case ArithmeticOperator::multiply: return "*";
                case ArithmeticOperator::divide: return "/";
            }
        }
        case 5: return ">";
        case 6: return ">=";
        case 7: return "<";
        case 8: return "<=";
        case 9: return "==";
        case 10: return "!=";
        case 11: return "&&";
        case 12: return "||";
        case 13: return "{";
        case 14: return "}";
        case 15: return ",";
        case 16: return std::get<Identifier>(token.kind).name;
        case 17: return std::to_string(std::get<Number>(token.kind).d);
        case 18: return std::string("Unknown char: ") + std::get<Unknown>(token.kind).c;
        case 19: return "EOF";
        default: return  "Unreachable";
    }
}


#endif //KALEIDOSCOPE_TOKEN_H
