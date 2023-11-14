//
// Created by gomil on 2023/11/12.
//

#ifndef KALEIDOSCOPE_AST_H
#define KALEIDOSCOPE_AST_H

#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <map>
#include <istream>
#include "bin_op.h"


enum class AstNodeType{
    number,
    functionCall,
    binaryExpr,
    variableReference,
    variableDeclaration,
};

class AstNode{
public:
    virtual ~AstNode() = default;
    virtual AstNodeType getType() const noexcept = 0;
};

class AstTree{
public:
    explicit AstTree(std::unique_ptr<AstNode> root): root(std::move(root)){
    }

    std::unique_ptr<AstNode> root;
};

class NumberNode: public AstNode{
    const double value;
public:
    explicit NumberNode(double value): value(value){}
    AstNodeType getType() const noexcept override{
        return AstNodeType::number;
    }
};

//calls a function
class FunctionCallNode: public AstNode{
    const std::string callee;
    const std::vector<std::unique_ptr<AstNode>> args;
public:
    FunctionCallNode(std::string callee,std::vector<std::unique_ptr<AstNode>> args): args(std::move(args)), callee(std::move(callee)){
    }
    AstNodeType getType() const noexcept override {
        return AstNodeType::functionCall;
    }
};

class BinaryExprNode: public AstNode {
    const BinOperator op;
    const std::unique_ptr<AstNode> lhs;
    const std::unique_ptr<AstNode> rhs;
public:
    explicit BinaryExprNode(BinOperator op, std::unique_ptr<AstNode> lhs, std::unique_ptr<AstNode> rhs):
    op(op), lhs(std::move(lhs)), rhs(std::move(rhs)){}
    AstNodeType getType() const noexcept override{
        return AstNodeType::binaryExpr;
    }
};

class VariableReferenceNode: public AstNode{
    const std::string id;
public:
    explicit VariableReferenceNode(std::string  id): id(std::move(id)){
    }
    AstNodeType getType() const noexcept override {
        return AstNodeType::variableReference;
    }
};


#endif //KALEIDOSCOPE_AST_H
