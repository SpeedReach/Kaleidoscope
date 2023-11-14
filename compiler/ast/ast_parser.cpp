//
// Created by gomil on 2023/11/12.
//

#include "include/ast.h"
#include "kale_error.h"
#include "include/ast_parser.h"




// a(f(b,c)+c)
// (f(b,c)+c)
// a+f(b,c)
ErrorOr<std::unique_ptr<AstNode>> Parser::ParseExpression() {

    while (!std::holds_alternative<EOFTok>(current.kind) && !std::holds_alternative<Semicolon>(current.kind)){
       // PrintState();
        if(parserElements.size() == 1 && std::holds_alternative<AstElement>(parserElements.back())){
            return std::get<AstElement>(std::move(parserElements.back())).expr;
        }
        // if '('
        if(std::holds_alternative<OpenParen>(current.kind)){
            BumpOpenParens();
            continue;
        }
        if(std::holds_alternative<Comma>(current.kind)){
            parserElements.emplace_back(CommaElement{});
            EatToken();
            continue;
        }
        // if is binary operator
        auto binOp = getOperator(current.kind);
        if(binOp.has_value()){
            int precedence = getPrecedence(*binOp);
            while (!operatorStack.empty() &&
            getPrecedence(operatorStack.back()) >= precedence ){
                auto r = LinkBinOp();
                if(!r.ok()){
                    return r.error();
                }
            }
            operatorStack.push_back(*binOp);
            EatToken();
            continue;
        }

        if(auto id = std::get_if<Identifier>(&current.kind)){
            auto name = std::string (id->name);
            EatToken();
            //push function to delimiter stack
            if(std::holds_alternative<OpenParen>(current.kind)){
                parserElements.emplace_back(FunctionElement{name});
                // eat '('
                EatToken();
            }
            else{
                parserElements.emplace_back(AstElement{std::make_unique<VariableReferenceNode>(id->name)});
            }
            continue;
        }

        if(auto num = std::get_if<Number>(&current.kind)){
            EatToken();
            parserElements.emplace_back(AstElement{std::make_unique<NumberNode>(num->d)});
            continue;
        }
        ///if ')' => link to last '(' or function 'f()'
        if(std::holds_alternative<CloseParen>(current.kind)){
            auto r = ParseParenOrFunction();
            if(!r.ok()){
                return r.error();
            }
            continue;
        }
        return std::string ("Unexpected token");
    }
    if(parserElements.size() != 1){
        return std::string ("failed to parse expression1");
    }
    auto result = std::move(parserElements.back());
    parserElements.pop_back();
    auto* node = std::get_if<AstElement>(&result);
    if(node == nullptr){
        return std::string ("failed to parse expression2");
    }
    return std::move(node->expr);
}

// possibles:
// ()  this is a language with only double as value, so this is not valid.
// )
// f) parserElements == 1 => err
// ,,) missing bin op
ErrorOr<Success> Parser::ParseParenOrFunction() {
    if(!std::holds_alternative<CloseParen>(current.kind)){
        return std::string ("should end with )");
    }
    EatToken();
    /// handle starting with only ')';
    if(parserElements.empty()){
        return std::string ("unexpected ')'");
    }
    ///handle "()",
    if(std::holds_alternative<OpenParenElement>(parserElements.back())){
        return std::string ("invalid syntax '()' ");
    }
    while (true){

        /// missing a '(' in the start.
        if(parserElements.size() == 1){
            return std::string("expected a '('");
        }
        ParserElement& prevElement = parserElements.at(parserElements.size()-2);
        //PrintState();


        /// When the left element is '(' we replace it with the right token.
        if(std::holds_alternative<OpenParenElement>(prevElement)){
            ParserElement element = std::move(parserElements.back());
            parserElements.pop_back();
            // prevElement becomes dangling reference.
            parserElements.pop_back();
            parserElements.emplace_back(std::move(element));
            return Success();
        }
        /// When the left element is ',' we assume it is a argument in a function call, and try parse it.
        if(std::holds_alternative<CommaElement>(prevElement)){
            auto result = ParseArgument();
            if(!result.ok()){
                return result.error();
            }
            /// eat comma, and [prevElement] is dangling rn.
            parserElements.pop_back();
            continue;
        }
        /// When the left element is a function ident, we place it in the fnArgStack, and start parsing the function.
        if(std::holds_alternative<FunctionElement>(prevElement)){
            auto result = ParseArgument();
            if(!result.ok()){
                return result.error();
            }
            auto f = std::get<FunctionElement>(std::move(parserElements.back()));
            parserElements.pop_back();
            std::string name = f.name;
            parserElements.emplace_back(AstElement{std::make_unique<FunctionCallNode>(name,std::move(fnArgStack))});
            ///no need to clear [fnArgStack], move already does it.
            return Success();
        }
        /// pop two AstElement and link them together with a binop => BinExpressionNode
        if(std::holds_alternative<AstElement>(prevElement)){
            auto result = LinkBinOp();
            if(!result.ok()){
                return result.error();
            }
            continue;
        }

        return std::string ("failed parsing");
    }
}

ErrorOr<Success> Parser::ParseArgument() {
    auto element = std::move(parserElements.back());
    parserElements.pop_back();
    if(auto ast = std::get_if<AstElement>(&element)){
        fnArgStack.emplace_back(std::move(ast->expr));
        return Success{};
    }
    else{
        return std::string ("Unexpected element");
    }
}


ErrorOr<Success> Parser::LinkBinOp() {
    if(operatorStack.empty()){
        return std::string ("expected a binary operator");
    }
    BinOperator binOpD = operatorStack.back();
    operatorStack.pop_back();
    if(parserElements.empty()){
        return std::string ("expected a expression on the right");
    }
    auto& element1 = parserElements.back();
    if(!std::holds_alternative<AstElement>(element1)){
        return std::string ("expected a expression on the right");
    }
    auto& element2 = parserElements.at(parserElements.size() -2);
    if(!std::holds_alternative<AstElement>(element2)){
        return std::string ("expected a expression on the right");
    }
    auto rExpr = std::get<AstElement>(std::move(parserElements.back()));
    parserElements.pop_back();
    auto lExpr = std::get<AstElement>(std::move(parserElements.back()));
    parserElements.pop_back();
    parserElements.emplace_back(AstElement{std::make_unique<BinaryExprNode>(binOpD,  std::move(lExpr.expr),std::move(rExpr.expr))});
    return Success();
}

ErrorOr<std::unique_ptr<AstTree>> Parser::ParseAstTree(){
    EatToken();
    std::unique_ptr<AstTree> tree;
    auto id = std::get_if<Identifier>(&current.kind);
    if(id != nullptr && id->name == "let"){
        auto result = ParseVariableDeclare();
        if(!result.ok()){
            return result.error();
        }
        tree = std::make_unique<AstTree>(std::move(*result));
    }
    else{
        auto result = ParseExpression();
        if(!result.ok()){
            return result.error();
        }
        tree = std::make_unique<AstTree>(std::move(*result));
    }
    //Check semicolon
    if(!std::holds_alternative<Semicolon>(current.kind)){
        return std::string("expected a semicolon");
    }
    //eat semicolon
    EatToken();
    return tree;
}

void Parser::BumpOpenParens() {
    while (std::holds_alternative<OpenParen>(current.kind)){
        parserElements.emplace_back(OpenParenElement{});
        EatToken();
    }
}


ErrorOr<std::unique_ptr<VariableDeclaration>> Parser::ParseVariableDeclare() {
    auto let = std::get_if<Identifier>(&current.kind);
    if(let == nullptr || let->name != "let"){
        return std::string("expected 'let'");
    }
    // eat let
    EatToken();
    auto ident = std::get_if<Identifier>(&current.kind);
    // eat ident
    EatToken();

    if(!std::holds_alternative<Assign>(current.kind)){
        return std::string("expected a '='");
    }
    // eat '='
    EatToken();

    auto expr = ParseExpression();
    if(!expr.ok()){
        return expr.error();
    }
    return std::make_unique<VariableDeclaration>(ident->name, std::move(*expr));
}


void Parser::EatToken() {
    current = cursor.AdvanceToken();
}










void Parser::PrintState() const noexcept {
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Current tok: "<< current.kind.index() << std::endl;
    std::cout << "fnArgStack: ";
    for(auto& arg: fnArgStack){
        std::cout << static_cast<int>(arg->getType()) << ",";
    }
    std::cout << std::endl << "parserElements: ";
    for(auto& element : parserElements){
        if(std::holds_alternative<OpenParenElement>(element)){
            std::cout << "OpenParen,";
        }
        else if(std::holds_alternative<FunctionElement>(element)){
            std::cout << "Function,";
        }
        else if(auto ast = std::get_if<AstElement>(&element)){
            std::cout << "Ast "<< static_cast<int>( ast->expr->getType()) << ",";
        }
        else if(std::holds_alternative<CommaElement>(element)){
            std::cout << "Comma,";
        }
    }
    std::cout << std::endl << "operatorStack: ";
    for (auto& op: operatorStack){
        std::cout << static_cast<int>(op) << ",";
    }
    std::cout << std::endl << "----------------------------------" << std::endl;
}
