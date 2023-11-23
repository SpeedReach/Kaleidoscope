//
// Created by gomil on 2023/11/22.
//

#ifndef KALEIDOSCOPE_FUNCTION_CONTEXT_H
#define KALEIDOSCOPE_FUNCTION_CONTEXT_H


#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include "ast_node.h"
#include "file_context.h"
#include "top_level.h"
#include <map>
#include <llvm/IR/IRBuilder.h>

class FunctionContext {

public:

    FunctionContext(FileContext* file_context,const std::unique_ptr<CallableImplementation>&  function):
    file_context(file_context),
    function(function),
    builder(file_context->llvm_context())
    {

    }


    ErrorOr<llvm::Function*> Lower();

    FileContext& Parent(){return *file_context;}
    llvm::AllocaInst* GetValue(const std::string& id){return namedValues[id];}
    void SetValue(const std::string& id, llvm::AllocaInst* alloc) {namedValues[id] = alloc;}
    llvm::IRBuilder<>& GetBuilder() {return builder;}
private:
    std::map<std::string, llvm::AllocaInst*> namedValues;

    // Context for the overall lowering process.
    FileContext* file_context;


    const std::unique_ptr<CallableImplementation>& function;
    // The generated IR after calling Lower()
    llvm::Function* generatedFn = nullptr;

    llvm::IRBuilder<> builder;

    ErrorOr<llvm::Function*> HandleDeclaration(CallableDeclaration& declaration);
};


#endif //KALEIDOSCOPE_FUNCTION_CONTEXT_H
