//
// Created by gomil on 2023/11/12.
//

#ifndef KALEIDOSCOPE_AST_NODE_H
#define KALEIDOSCOPE_AST_NODE_H

#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <istream>
#include <variant>
#include "bin_op.h"
#include "kale_error.h"



enum class AstNodeType{
    number,
    functionCall,
    binaryExpr,
    variableReference
};

class NumberNode;
class FunctionCallNode;
class BinaryExprNode;
class VariableReferenceNode;

class AstNode{
public:
    virtual ~AstNode() = default;
    virtual AstNodeType getType() const  noexcept = 0;
};

class NumberNode: public AstNode {

public:
    const double value;

    explicit NumberNode(double value): value(value){}

    AstNodeType getType() const noexcept override{
        return AstNodeType::number;
    }
};


//calls a function
class FunctionCallNode: public AstNode {
public:
    FunctionCallNode(std::string callee,std::vector<std::unique_ptr<AstNode>> args): args(std::move(args)), callee(std::move(callee)){
    }
    AstNodeType getType()  const noexcept override{
        return AstNodeType::functionCall;
    }

    const std::string callee;
    const std::vector<std::unique_ptr<AstNode>> args;
};

class BinaryExprNode : public AstNode {
public:
    explicit BinaryExprNode(BinOperator op, std::unique_ptr<AstNode> lhs,std::unique_ptr<AstNode> rhs):
    op(op), lhs(std::move(lhs)), rhs(std::move(rhs)){}

    AstNodeType getType()  const noexcept override {
        return AstNodeType::binaryExpr;
    }
    const BinOperator op;

    std::unique_ptr<AstNode> lhs;
    std::unique_ptr<AstNode> rhs;
};

class VariableReferenceNode: public AstNode {

public:
    const std::string id;
    explicit VariableReferenceNode(std::string  id): id(std::move(id)){
    }
    AstNodeType getType()  const noexcept override {
        return AstNodeType::variableReference;
    }
};


#endif //KALEIDOSCOPE_AST_NODE_H
