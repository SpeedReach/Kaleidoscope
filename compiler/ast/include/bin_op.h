//
// Created by gomil on 2023/11/13.
//

#ifndef KALEIDOSCOPE_BIN_OP_H
#define KALEIDOSCOPE_BIN_OP_H

#include <optional>
#include <variant>
#include "token.h"

enum class BinOperator{
    // +
    add,
    // -
    subtract,
    // *
    multiply,
    // /
    divide,
    // >
    greater,
    // >=
    greaterEqual,
    // <
    less,
    // <=
    lessEqual,
    // ==
    equalEqual,
    // !=
    notEqual,
    // &&
    andAnd,
    // ||
    orOr
};

static int getPrecedence(BinOperator op){
    switch (op) {
        case BinOperator::add:
        case BinOperator::subtract: return 20;
        case BinOperator::multiply:
        case BinOperator::divide: return 40;
        case BinOperator::greater:
        case BinOperator::greaterEqual:
        case BinOperator::less:
        case BinOperator::lessEqual:
        case BinOperator::equalEqual:
        case BinOperator::notEqual:
        case BinOperator::andAnd:
        case BinOperator::orOr: return 0;
    }
    return  -1;
}

static std::optional<BinOperator> getOperator(const TokenKind& kind){
    if(auto arith = std::get_if<Arithmetic>(&kind)){
        switch (arith->op) {
            case ArithmeticOperator::add: return BinOperator::add;
            case ArithmeticOperator::subtract: return BinOperator::subtract;
            case ArithmeticOperator::multiply: return BinOperator::multiply;
            case ArithmeticOperator::divide: return BinOperator::divide;
        }
    }
    if(std::holds_alternative<GreaterThan>(kind)) {
        return BinOperator::greater;
    }
    if(std::holds_alternative<GreaterEqual>(kind)) {
        return BinOperator::greaterEqual;
    }
    if(std::holds_alternative<LessThan>(kind)) {
        return BinOperator::less;
    }
    if(std::holds_alternative<LessEqual>(kind)) {
        return BinOperator::lessEqual;
    }
    if(std::holds_alternative<EqualEqual>(kind)) {
        return BinOperator::equalEqual;
    }
    if(std::holds_alternative<NotEqual>(kind)) {
        return BinOperator::notEqual;
    }
    if(std::holds_alternative<AndAnd>(kind)) {
        return BinOperator::andAnd;
    }
    if(std::holds_alternative<OrOr>(kind)) {
        return BinOperator::orOr;
    }
    return std::nullopt;
}

#endif //KALEIDOSCOPE_BIN_OP_H
