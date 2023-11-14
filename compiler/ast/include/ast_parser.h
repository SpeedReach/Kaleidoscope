//
// Created by gomil on 2023/11/13.
//

#ifndef KALEIDOSCOPE_AST_PARSER_H
#define KALEIDOSCOPE_AST_PARSER_H

#include "token.h"
#include "cursor.h"
#include "ast.h"
#include "kale_error.h"
#include "declaration.h"
#include <variant>


class Parser{
private:
    struct OpenParenElement{};
    struct CommaElement{};
    struct FunctionElement{
        std::string name;
    };
    struct AstElement{
        std::unique_ptr<AstNode> expr;
    };
    using ParserElement = std::variant<OpenParenElement,FunctionElement,AstElement,CommaElement>;

    Token current = Token{EOFTok{},0 };
    Cursor cursor;

    std::vector<std::unique_ptr<AstNode>> fnArgStack;
    std::vector<ParserElement> parserElements;
    std::vector<BinOperator> operatorStack;

    void PrintState() const noexcept;

    /**
     * Get the next token and save in current
     */
    void EatToken();

    void BumpOpenParens();
    ErrorOr<Success> LinkBinOp();
    // Is called when the current Token is ')'
    ErrorOr<Success> ParseParenOrFunction();
    ErrorOr<Success> ParseArgument();
    /**
     * ParseAstTree a variable declaration  \n
     * let a = 10 \n
     * note: does not contain semicolon
     */
    ErrorOr<std::unique_ptr<VariableDeclaration>> ParseVariableDeclare();
    /**
     * ParseAstTree a expression  \n
     * a+b   \n
     * f(a,c,f(d,e,3>g+3*4))+b   \n
     * f(a,c,f(d,e,3>g+(3*4)))+b   \n
     * (f(a,c)+b)   \n
     */
    ErrorOr<std::unique_ptr<AstNode>> ParseExpression();


public:
    explicit Parser(Cursor& cursor): cursor(cursor){

    }

    /**
     * ParseAstTree an expression ended with a semicolon  \n
     * let a = c+b;
     * a = c+b; \n
     * a = 10;  \n
     * a = dd(ff(b,c) + c);  \n
     * d(a,b,c); \n
     */
    ErrorOr<std::unique_ptr<AstTree>> ParseAstTree();

};



#endif //KALEIDOSCOPE_AST_PARSER_H
