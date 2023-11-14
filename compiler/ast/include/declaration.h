//
// Created by gomil on 2023/11/14.
//

#ifndef KALEIDOSCOPE_DECLARATION_H
#define KALEIDOSCOPE_DECLARATION_H

#include "ast.h"

class Declaration {
public:
    virtual const std::string& getIdentifier() const noexcept = 0;
};


//prototype.
class CallableDeclaration: public Declaration {

    const std::string name;
    const std::vector<std::string> args;

public:
    explicit CallableDeclaration(std::string name,std::vector<std::string> args):
            name(std::move(name)), args(std::move(args)){
    }

    const std::string& getIdentifier() const noexcept override;
};

class BlockNode {
    const std::vector<std::unique_ptr<AstTree>> expressions;
public:
    explicit BlockNode(std::vector<std::unique_ptr<AstTree>> expressions): expressions(std::move(expressions)){}
};

//implementation.
class CallableImplementation: public Declaration{
    const std::string name;
    const std::vector<std::string> args;
    const std::unique_ptr<BlockNode> body;
public:
    explicit CallableImplementation(std::string name,std::vector<std::string> args,std::unique_ptr<BlockNode> body):
            name(std::move(name)), args(std::move(args)), body(std::move(body)){}

    const std::string& getIdentifier() const noexcept override;
};

class VariableDeclaration: public Declaration, public AstNode {
    const std::string identifier;
    const std::unique_ptr<AstNode> expr;
public:
    explicit VariableDeclaration(std::string identifier,std::unique_ptr<AstNode> expr): identifier(std::move(identifier)),
                                                                                        expr(std::move(expr)){}
    const std::string& getIdentifier() const noexcept override;
    AstNodeType getType() const noexcept override{
        return AstNodeType::variableDeclaration;
    }
};

#endif //KALEIDOSCOPE_DECLARATION_H
