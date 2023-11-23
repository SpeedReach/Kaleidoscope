//
// Created by gomil on 2023/11/12.
//


#include "include/ast_parser.h"
#include "ast_node.h"
#include <memory>


ErrorOr<std::unique_ptr<Ast>> AstParser::ParseAstTree(){
    std::unique_ptr<Ast> ast;
    auto id = std::get_if<Identifier>(&reader.GetCurrentToken().kind);
    // let a = 10;
    if(id != nullptr && id->name == "let"){
        auto result = ParseVariableDeclare();
        if(!result.ok()){
            return result.error();
        }
        ast = std::move(*result);
    }
        // return a;
    else if(id != nullptr && id->name == "return"){
        //eat return
        Bump();
        auto result = ParseExpression();
        if(!result.ok()){
            return result.error();
        }
        ast = std::make_unique<ReturnAst>(std::move(*result));
    }
    else{
        // a = 10;
        if(std::holds_alternative<Identifier>(reader.GetCurrentToken().kind) &&
           std::holds_alternative<Assign>(reader.GetNextToken().kind)
                ){
            auto varName = std::get_if<Identifier>(&reader.GetCurrentToken().kind)->name;
            //remove a and =
            Bump();
            Bump();
            auto result = ParseExpression();
            if(!result.ok()){
                return result.error();
            }
            ast = std::make_unique<AssignmentAst>(std::make_unique<VariableReferenceNode>(varName),std::move(*result));
        }
        // pure expression
        else{
            auto result1 = HandleToken();
            Bump();
            if(!result1.ok()){
                return result1.error();
            }
            auto result2 = ParseExpression();
            if(!result2.ok()){
                return result2.error();
            }
            ast = std::make_unique<ExpressionAst>(std::move(*result2));
        }
    }
    //Check semicolon
    if(!std::holds_alternative<Semicolon>(reader.GetCurrentToken().kind)){
        return std::string("expected a semicolon");
    }
    //eat semicolon
    Bump();
    return ast;
}

// a(f(b,c)+c)
// (f(b,c)+c)
// a+f(b,c)
ErrorOr<std::unique_ptr<AstNode>> AstParser::ParseExpression() {
    while (!std::holds_alternative<EOFTok>(reader.GetCurrentToken().kind) && !std::holds_alternative<Semicolon>(reader.GetCurrentToken().kind)){
        auto r = HandleToken();
        if(!r.ok()){
            return r.error();
        }
        Bump();
    }
    //Try link all the remaining binOp
    while (parserElements.size() >= 2 && !operatorStack.empty()){
        auto r = LinkBinOp();
        PrintState();
        if(!r.ok()){
            return r.error();
        }
    }
    //Check if the expression is valid
    if(parserElements.size() != 1){
        return std::string ("failed to Parse expression1");
    }

    auto result = std::move(parserElements.back());
    parserElements.pop_back();
    auto* node = std::get_if<AstElement>(&result);
    if(node == nullptr){
        return std::string ("failed to Parse expression2");
    }
    return std::move(node->expr);
}

// possibles:
// ()  this is a language with only double as value, so this is not valid.
// )
// f) parserElements == 1 => err
// ,,) missing bin op
ErrorOr<Success> AstParser::ParseParenOrFunction() {
    if(!std::holds_alternative<CloseParen>(reader.GetCurrentToken().kind)){
        return std::string ("should end with )");
    }
    /// handle starting with only ')';
    if(parserElements.empty()){
        return std::string ("unexpected ')'");
    }
    ///handle "()",
    if(std::holds_alternative<OpenParenElement>(parserElements.back())){
        return std::string ("invalid syntax '()' ");
    }
    while (true){
        PrintState();
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
        /// When the left element is ',' we assume it is a argument in a function call, and try Parse it.
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
            // Parse the most left element to an argument
            auto result = ParseArgument();
            PrintState();
            if(!result.ok()){
                return result.error();
            }
            auto f = std::get<FunctionElement>(std::move(parserElements.back()));
            parserElements.pop_back();
            std::string name = f.name;
            //reverse the argument stack, since we parsed it from right to left.
            std::reverse(fnArgStack.begin(), fnArgStack.end());
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

ErrorOr<Success> AstParser::ParseArgument() {
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


ErrorOr<Success> AstParser::LinkBinOp() {
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



ErrorOr<std::unique_ptr<VariableDeclareAst>> AstParser::ParseVariableDeclare() {
    auto let = std::get_if<Identifier>(&reader.GetCurrentToken().kind);
    if(let == nullptr || let->name != "let"){
        return std::string("expected 'let'");
    }
    // eat let
    Bump();
    auto ident = std::get_if<Identifier>(&reader.GetCurrentToken().kind)->name;
    // eat ident
    Bump();

    if(!std::holds_alternative<Assign>(reader.GetCurrentToken().kind)){
        return std::string("expected a '='");
    }
    // eat '='
    Bump();

    auto expr = ParseExpression();
    if(!expr.ok()){
        return expr.error();
    }
    return std::make_unique<VariableDeclareAst>(ident, std::move(*expr));
}


void AstParser::Bump() {
    std::cout << "Bumping " << ToString(reader.GetCurrentToken()) << std::endl;
    reader.Forward();
}






void AstParser::PrintState() const noexcept {
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Current tok: " << ToString(reader.GetCurrentToken()) << std::endl;
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

ErrorOr<Success> AstParser::HandleToken() {
    PrintState();
    // if '('
    if(std::holds_alternative<OpenParen>(GetCurrentToken().kind)){
        parserElements.emplace_back(OpenParenElement{});
        return Success();
    }
    if(std::holds_alternative<Comma>(GetCurrentToken().kind)){
        parserElements.emplace_back(CommaElement{});
        return Success();
    }
    // if is binary operator
    auto binOp = getOperator(GetCurrentToken().kind);
    if(binOp.has_value()){
        int precedence = getPrecedence(*binOp);
        while (!operatorStack.empty() &&
               getPrecedence(operatorStack.back()) >= precedence ){
            std::cout<<"1";
            auto r = LinkBinOp();
            if(!r.ok()){
                return r.error();
            }
        }
        operatorStack.push_back(*binOp);
        return Success();
    }
    if(auto id = std::get_if<Identifier>(&reader.GetCurrentToken().kind)){
        //store name before eat
        auto name = std::string (id->name);
        //push function to delimiter stack
        if(std::holds_alternative<OpenParen>(GetNextToken().kind)){
            // eat 'name'
            Bump();
            parserElements.emplace_back(FunctionElement{name});
        }
        else{
            parserElements.emplace_back(AstElement{std::make_unique<VariableReferenceNode>(name)});
        }
        return  Success();
    }

    if(auto num = std::get_if<Number>(&reader.GetCurrentToken().kind)){
        parserElements.emplace_back(AstElement{std::make_unique<NumberNode>(num->d)});
        return Success();
    }
    ///if ')' => link to last '(' or function 'f()'
    if(std::holds_alternative<CloseParen>(reader.GetCurrentToken().kind)){
        auto r = ParseParenOrFunction();
        if(!r.ok()){
            return r.error();
        }
        return Success();
    }
    PrintState();
    return std::string ("Unexpected token");
}
