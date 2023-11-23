//
// Created by gomil on 2023/11/20.
//
#include "include/file_parser.h"
#include "include/ast_parser.h"
#include "top_level.h"
#include "kale_error.h"
#include "token.h"


ErrorOr<std::unique_ptr<FileAst>> FileParser::Parse(){
    std::vector<std::unique_ptr<CallableDeclaration>> funcPrototype;
    std::vector<std::unique_ptr<CallableImplementation>> functionImpl;
    while (!std::holds_alternative<EOFTok>(GetCurrentToken().kind)){
        std::cout<<"22222222222222";
        auto prototype = parseProtoType();
        if(!prototype.ok()){
            return prototype.error();
        }
        if(std::get_if<OpenBracket>(&GetCurrentToken().kind)){
            auto block = parseBody();
            if(!block.ok()){
                return block.error();
            }
            functionImpl.push_back( std::make_unique<CallableImplementation>(std::move(*prototype),std::move(*block)));
        }
        else{
            std::unique_ptr<CallableDeclaration> uProto = std::move(*prototype);
            funcPrototype.push_back(std::move(uProto));
        }
    }
    return std::make_unique<FileAst>(std::move(funcPrototype), std::move(functionImpl));
}
/// fn a(a,b)
ErrorOr<std::unique_ptr<CallableDeclaration>> FileParser::parseProtoType(){
    auto fn = std::get_if<Identifier>(&GetCurrentToken().kind);
    if(!fn || fn->name != "fn"){
        return std::string ("functions should starts with fn");
    }
    Bump();
    auto id = std::get_if<Identifier>(&GetCurrentToken().kind);
    if(!id){
        return std::string ("missing a function name");
    }
    auto name = id->name;
    Bump();
    if(!std::get_if<OpenParen>(&GetCurrentToken().kind)){
        return std::string ("expecting a open paren");
    }
    Bump();
    std::vector<std::string> args;
    while (true){
        auto arg = std::get_if<Identifier>(&GetCurrentToken().kind);
        if(!arg){
            return std::string("missing a arg name");
        }
        args.push_back(arg->name);
        Bump();
        if(std::holds_alternative<Comma>(GetCurrentToken().kind)) {
            Bump();
            continue;
        }
        if(std::holds_alternative<CloseParen>(GetCurrentToken().kind)) {
            Bump();
            break;
        }
        else return std::string ("expected a close paren");
    }
    return std::make_unique<CallableDeclaration>(name,args);
}

void FileParser::Bump() {
    reader.Forward();
}

ErrorOr<std::unique_ptr<Block>> FileParser::parseBody() {
    if(!std::get_if<OpenBracket>(&GetCurrentToken().kind)){
        return std::string ("expecting a open bracket");
    }
    Bump();
    std::vector<std::unique_ptr<Ast>> trees;
    while (!std::get_if<CloseBracket>(&GetCurrentToken().kind)){
        AstParser semiParser(reader);
        auto result = semiParser.ParseAstTree();
        if(!result.ok()){
            return  result.error();
        }
        trees.push_back(std::move(*result));
    }
    Bump();
    return std::make_unique<Block>(std::move(trees));
}
