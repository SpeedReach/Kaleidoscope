//
// Created by gomil on 2023/11/14.
//

#include <gtest/gtest.h>
#include "ast_parser.h"
#include "ast_node.h"
#include "ast.h"
#include "kale_error.h"
#include "cursor.h"
#include "file_parser.h"

TEST(ParserTest, BinOpPrecedenceTest){
    EXPECT_EQ(getPrecedence(BinOperator::add), 20);
    EXPECT_EQ(getPrecedence(BinOperator::subtract), 20);
    EXPECT_EQ(getPrecedence(BinOperator::multiply), 40);
    EXPECT_EQ(getPrecedence(BinOperator::divide), 40);
    EXPECT_EQ(getPrecedence(BinOperator::greater), 0);
    EXPECT_EQ(getPrecedence(BinOperator::greaterEqual), 0);
    EXPECT_EQ(getPrecedence(BinOperator::less), 0);
    EXPECT_EQ(getPrecedence(BinOperator::lessEqual), 0);
    EXPECT_EQ(getPrecedence(BinOperator::equalEqual), 0);
    EXPECT_EQ(getPrecedence(BinOperator::notEqual), 0);
    EXPECT_EQ(getPrecedence(BinOperator::andAnd), 0);
    EXPECT_EQ(getPrecedence(BinOperator::orOr), 0);
}



TEST(AstTest, TestTypes ){
    EXPECT_EQ(BinaryExprNode(BinOperator::divide,std::make_unique<NumberNode>(2),std::make_unique<NumberNode>(2)).getType(),AstNodeType::binaryExpr);
    EXPECT_EQ(NumberNode(2).getType(),AstNodeType::number);
    EXPECT_EQ(FunctionCallNode("f",std::vector<std::unique_ptr<AstNode>>()).getType(),AstNodeType::functionCall);
    EXPECT_EQ(VariableReferenceNode("a").getType(),AstNodeType::variableReference);
}

TEST(ParseTreeTest, ParseSuccess1){
    std::string code = "let aw = dd(ff(b,c) + e);";
    std::string_view view(code);
    StringReader reader(view);
    AstParser parser(reader);
    auto result = parser.ParseAstTree();
    EXPECT_TRUE(result.ok()) << result.error();
    std::unique_ptr<Ast> ast = std::move(*result);
    auto pVariableDeclareAst = dynamic_cast<VariableDeclareAst*>(ast.get());
    EXPECT_EQ(pVariableDeclareAst->identifier, "aw");
    auto funcCall = dynamic_cast<FunctionCallNode*>(pVariableDeclareAst->expr.get());
    EXPECT_EQ(funcCall->callee, "dd");
    auto binNode = dynamic_cast<BinaryExprNode*>(funcCall->args[0].get());
    EXPECT_EQ(binNode->op, BinOperator::add);
    auto fCall2 = dynamic_cast<FunctionCallNode*>(binNode->lhs.get());
    EXPECT_EQ(fCall2->callee, "ff");
    auto fCall2Arg1 = dynamic_cast<VariableReferenceNode*>( fCall2->args[0].get());
    EXPECT_EQ(fCall2Arg1->id,"b");
    auto fCall2Arg2 = dynamic_cast<VariableReferenceNode*>( fCall2->args[1].get());
    EXPECT_EQ(fCall2Arg2->id,"c");
    auto varRef2 = dynamic_cast<VariableReferenceNode*>(binNode->rhs.get());
    EXPECT_EQ(varRef2->id, "e");
}

TEST(ParseTreeTest, ParseSuccess2){
    std::string  code = "a=b+c;";
    std::string_view view(code);
    StringReader reader = StringReader(view);
    AstParser parser(reader);
    auto result = parser.ParseAstTree();
    EXPECT_TRUE(result.ok()) << result.error() << " index: "<< ToString( reader.GetNextToken()) << "\n";
    std::unique_ptr<Ast> ast = std::move(*result);
    auto pAssignmentAst = dynamic_cast<AssignmentAst*>(ast.get());
    auto pBinaryExprNode = dynamic_cast<BinaryExprNode*>(pAssignmentAst->expr.get());
    EXPECT_EQ(pBinaryExprNode->op, BinOperator::add);
}
TEST(ParseTreeTest, ParseSuccess3){
    std::string code = "b+c;";
    std::string_view view(code);
    StringReader cursor(view);
    AstParser parser(cursor);
    auto result = parser.ParseAstTree();
    EXPECT_TRUE(result.ok()) << result.error();
    std::unique_ptr<Ast> ast = std::move(*result);
    auto pAst= dynamic_cast<ExpressionAst*>(ast.get());
    auto binNode = dynamic_cast<BinaryExprNode*>(pAst->expr.get());
    EXPECT_EQ(binNode->op, BinOperator::add);
}
TEST(ParseFileTest, ParseSuccess1){
    std::string code = "fn aw(a,b,c) { a=b+c; return a + b + c; }";
    std::string_view view(code);
    FileParser parser(view);
    auto result = parser.Parse();
    EXPECT_TRUE(result.ok());
    std::unique_ptr<FileAst> fileContext = std::move(*result);
    auto f =  fileContext->functionImpl[0].get();

    EXPECT_EQ(f->declaration->name, "aw");
    EXPECT_EQ(f->declaration->args.size(),3);
    EXPECT_EQ(f->declaration->args[0], "a");
    EXPECT_EQ(f->declaration->args[1], "b");
    EXPECT_EQ(f->declaration->args[2], "c");
    auto body = f->body.get();
    EXPECT_TRUE(dynamic_cast<AssignmentAst*>(body->trees[0].get()));
    EXPECT_TRUE(dynamic_cast<ReturnAst*>(body->trees[1].get()));
}


