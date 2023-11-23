#include <iostream>
#include "compiler/parsers/include/ast_parser.h"
#include "compiler/parsers/include/file_parser.h"
#include "compiler/codegen/include/file_context.h"
#include "compiler/ast/include/top_level.h"
#include "compiler/codegen/include/function_context.h"

int main() {
    std::string code = "fn aw(a,b,c) { a=b+c; return a + b + c; }";
    std::string_view view(code);
    FileParser parser(view);
    auto parsedFile = parser.Parse();
    FileContext context;
    const std::unique_ptr<CallableImplementation>& fRef = (*parsedFile)->functionImpl[0];
    FunctionContext functionContext(&context,fRef);
    auto result = functionContext.Lower();
    std::cout << result.error();
    functionContext.Parent().llvm_module().print(llvm::errs(), nullptr);
    return 0;
}
