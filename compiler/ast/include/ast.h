//
// Created by gomil on 2023/11/20.
//

#ifndef KALEIDOSCOPE_AST_TREE_H
#define KALEIDOSCOPE_AST_TREE_H

#include "ast_node.h"

enum AstType{
    ret,
    var,
    expr,
    assign
};

class Ast{
public:
    virtual ~Ast() = default;
    virtual AstType getType() const noexcept = 0;
};

class ReturnAst:public Ast{
public:
    const std::unique_ptr<AstNode> expr;
    explicit ReturnAst(std::unique_ptr<AstNode> expr): expr(std::move(expr)){
    }
    AstType getType() const noexcept override{
        return AstType::ret;
    }
};

class VariableDeclareAst: public Ast{
public:
    const std::string identifier;
    const std::unique_ptr<AstNode> expr;
    explicit VariableDeclareAst(std::string identifier,std::unique_ptr<AstNode> expr): identifier(std::move(identifier)),
                                                                                        expr(std::move(expr)){
    }

    AstType getType() const noexcept override{
        return AstType::var;
    }
};

class ExpressionAst: public Ast{
public:
    const std::unique_ptr<AstNode> expr;
    explicit ExpressionAst(std::unique_ptr<AstNode> expr): expr(std::move(expr)){
    }
    AstType getType() const noexcept override{
        return AstType::expr;
    }
};

class AssignmentAst: public Ast{
public:
    const std::unique_ptr<VariableReferenceNode> assignee;

    const std::unique_ptr<AstNode> expr;

    explicit AssignmentAst(std::unique_ptr<VariableReferenceNode>  assignee,std::unique_ptr<AstNode> expr): assignee(std::move(assignee)),
                                                                                   expr(std::move(expr)){
    }
    AstType getType() const noexcept override{
        return AstType::assign;
    }

};

#endif //KALEIDOSCOPE_AST_TREE_H
