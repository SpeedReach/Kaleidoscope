#include "gtest/gtest.h"
#include "file_parser.h"
#include "file_context.h"
#include "function_context.h"
#include "handle.h"


TEST(LowerFile, LowerSingleFunction){
    std::string code = "fn aw(a,b,c) { a=b+c; return a + b + c; }";
    std::string_view view(code);
    FileParser parser(view);
    auto parsedFile = parser.Parse();
    EXPECT_TRUE(parsedFile.ok()) << parsedFile.error();
    FileContext context;
    const std::unique_ptr<CallableImplementation>& fRef = (*parsedFile)->functionImpl[0];
    FunctionContext functionContext(&context,fRef);
    auto result = functionContext.Lower();
    EXPECT_TRUE(result.ok()) << result.error();
    functionContext.Parent().llvm_module().print(llvm::errs(), nullptr);
}


TEST(LowerFile, UnitTest){
    std::string code = "fn aw(a,b,c) { a=b+c; return a + b + c; }";
    std::string_view view(code);
    FileParser parser(view);
    auto parsedFile = parser.Parse();
    EXPECT_TRUE(parsedFile.ok()) << parsedFile.error();
    FileContext context;
    const std::unique_ptr<CallableImplementation>& fRef = (*parsedFile)->functionImpl[0];
    FunctionContext functionContext(&context,fRef);

    auto a = std::make_unique<NumberNode>(3);
    auto b = std::make_unique<NumberNode>(4);

    auto aV = HandleNumber(functionContext,*a);
    auto bV = HandleNumber(functionContext, *b);

    std::cout << *aV << std::endl;
    std::cout << *bV << std::endl;
}


