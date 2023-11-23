//
// Created by gomil on 2023/11/13.
//

#ifndef KALEIDOSCOPE_AST_PARSER_H
#define KALEIDOSCOPE_AST_PARSER_H

#include "token.h"
#include "cursor.h"
#include "ast_node.h"
#include "kale_error.h"
#include "top_level.h"
#include "string_reader.h"
#include <variant>

struct OpenParenElement{};
struct CommaElement{};
struct FunctionElement{
    std::string name;
};
struct AstElement{
    std::unique_ptr<AstNode> expr;
};
using ParserElement = std::variant<OpenParenElement,FunctionElement,AstElement,CommaElement>;

class AstParser{
public:
    explicit AstParser(StringReader& reader): reader(reader){
    }
    /**
     * ParseAstTree an expression ended with a semicolon  \n
     * let a = c+b;
     * a = c+b; \n
     * a = 10;  \n
     * a = dd(ff(b,c) + c);  \n
     * d(a,b,c); \n
     */
    ErrorOr<std::unique_ptr<Ast>> ParseAstTree();
private:

    StringReader& reader;
    std::vector<std::unique_ptr<AstNode>> fnArgStack;
    std::vector<ParserElement> parserElements;
    std::vector<BinOperator> operatorStack;
    /**
     * For debug purposes.
     */
    void PrintState() const noexcept;

    const Token& GetCurrentToken() const noexcept{
        return reader.GetCurrentToken();
    }

    const Token& GetNextToken() const noexcept{
        return reader.GetNextToken();
    }

    void Bump();

    /**
     * Take the last two elements in [parserElements] and the last element in operatorStack
     * and link them as a [AstElement{BinOpExpr}] and save in [parserElements]
     */
    ErrorOr<Success> LinkBinOp();
    // Is called when the current Token is ')'
    ErrorOr<Success> ParseParenOrFunction();

    /**
     * Save current in fnArgStack
     */
    ErrorOr<Success> ParseArgument();
    /**
     * ParseAstTree a variable declaration  \n
     * let a = 10 \n
     * note: does not contain semicolon
     */
    ErrorOr<std::unique_ptr<VariableDeclareAst>> ParseVariableDeclare();
    /**
     * ParseAstTree a expression  \n
     * a+b   \n
     * f(a,c,f(d,e,3>g+3*4))+b   \n
     * f(a,c,f(d,e,3>g+(3*4)))+b   \n
     * (f(a,c)+b)   \n
     */
    ErrorOr<std::unique_ptr<AstNode>> ParseExpression();

    ErrorOr<Success> HandleToken();
};



#endif //KALEIDOSCOPE_AST_PARSER_H
