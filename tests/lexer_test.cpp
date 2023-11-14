#include <gtest/gtest.h>
#include <iostream>
#include <variant>
#include "cursor.h"



// Demonstrate some basic assertions.
TEST(LexerTest, BasicAssertions) {
    std::string str = "fn fib(x)\n"
                      "  if x <= 23.33 then\n"
                      "    1\n"
                      "  else\n"
                      "    fib(x-1)+fib(x-2)";

    std::string_view src(str);

    Cursor cursor(src);
    Token tok;

    //def
    tok = cursor.AdvanceToken();
    EXPECT_EQ(std::get<Identifier>(tok.kind).name,"fn");
    //fib
    tok = cursor.AdvanceToken();
    EXPECT_EQ(std::get<Identifier>(tok.kind).name,"fib");
    // (
    tok = cursor.AdvanceToken();
    EXPECT_TRUE(std::holds_alternative<OpenParen>(tok.kind)) ;
    //x
    tok = cursor.AdvanceToken();
    EXPECT_EQ(std::get<Identifier>(tok.kind).name,"x") ;
    // )
    tok = cursor.AdvanceToken();
    EXPECT_TRUE(std::holds_alternative<CloseParen>(tok.kind));
    //if
    tok = cursor.AdvanceToken();
    EXPECT_EQ(std::get<Identifier>(tok.kind).name,"if") ;
    //x
    tok = cursor.AdvanceToken();
    EXPECT_TRUE(std::holds_alternative<Identifier>(tok.kind)) ;
    // <
    tok = cursor.AdvanceToken();
    EXPECT_TRUE(std::holds_alternative<LessEqual>(tok.kind)) ;

    // 3
    tok = cursor.AdvanceToken();
    EXPECT_EQ(std::get<Number>(tok.kind).d,23.33);
    // then
    tok = cursor.AdvanceToken();
    EXPECT_TRUE(std::holds_alternative<Identifier>(tok.kind));
}