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

template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overload(Ts...) -> overload<Ts...>;


#endif //KALEIDOSCOPE_TOKEN_H
