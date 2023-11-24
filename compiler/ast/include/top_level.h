//
// Created by gomil on 2023/11/14.
//

#ifndef KALEIDOSCOPE_TOP_LEVEL_H
#define KALEIDOSCOPE_TOP_LEVEL_H

#include "ast_node.h"
#include "ast.h"
#include "kale_error.h"


//prototype.
class CallableDeclaration {
public:
    const std::string name;
    const std::vector<std::string> args;

    explicit CallableDeclaration(std::string name,std::vector<std::string> args):
            name(std::move(name)), args(std::move(args)){
    }

};

class Block {

public:
    const std::vector<std::unique_ptr<Ast>> trees;
    explicit Block(std::vector<std::unique_ptr<Ast>> expressions): trees(std::move(expressions)){}
};

//implementation.
class CallableImplementation {
public:
    const std::unique_ptr<CallableDeclaration> declaration;
    const std::unique_ptr<Block> body;

    explicit CallableImplementation( std::unique_ptr<CallableDeclaration> declaration,std::unique_ptr<Block> body):
            declaration(std::move(declaration)), body(std::move(body)){}
};


class FileAst{
public:
    const std::vector<std::unique_ptr<CallableDeclaration>> funcPrototype;
    const std::vector<std::unique_ptr<CallableImplementation>> functionImpl;
    explicit FileAst(std::vector<std::unique_ptr<CallableDeclaration>> funcPrototype,
                     std::vector<std::unique_ptr<CallableImplementation>> functionImpl):
            funcPrototype(std::move(funcPrototype)), functionImpl(std::move(functionImpl)){}
};


#endif //KALEIDOSCOPE_TOP_LEVEL_H
