//
// Created by gomil on 2023/11/14.
//

#include <gtest/gtest.h>
#include "ast_parser.h"
#include "ast.h"
#include "kale_error.h"
#include "cursor.h"

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

TEST(ParserTest, ParseSuccess1){
    std::string code = "let aw = dd(ff(b,c) + c);";
    std::string_view view(code);
    Cursor cursor(view);
    Parser parser(cursor);
    auto result = parser.ParseAstTree();
    EXPECT_TRUE(result.ok()) << result.error();
    std::unique_ptr<AstTree> ast = std::move(*result);
    EXPECT_EQ(ast->root->getType(), AstNodeType::variableDeclaration);
}