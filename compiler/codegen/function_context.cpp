//
// Created by gomil on 2023/11/23.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include "include/function_context.h"
#include "include/file_context.h"
#include "include/handle.h"

ErrorOr<llvm::Function *> FunctionContext::Lower() {
    if(generatedFn){
        return generatedFn;
    }
    auto fn = file_context->llvm_module().getFunction(function->declaration->name);
    if(!fn) {
        auto r =  HandleDeclaration(*function->declaration);
        if(!r.ok()){
            return r.error();
        }
        fn = *r;
    }
    else if (!fn->empty()) {
        return std::string("Function cannot be redefined.");
    }
    generatedFn = fn;
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(file_context->llvm_context(), "entry", generatedFn);
    builder.SetInsertPoint(BB);
    for (auto &arg : generatedFn->args()){
        llvm::IRBuilder<> TmpB(&generatedFn->getEntryBlock(),
                         generatedFn->getEntryBlock().begin());
        auto alloc = TmpB.CreateAlloca(llvm::Type::getDoubleTy(file_context->llvm_context()), nullptr, arg.getName());
        namedValues[arg.getName().str()] = alloc;
    }

    bool hasReturn = false;
    for (auto &ast : function->body->trees ){
        switch (ast->getType()) {
            case AstType::ret: {
                auto ret = dynamic_cast<ReturnAst*>(ast.get());
                auto v = HandleAstNode(*this,*ret->expr);
                if(!v.ok()){
                    return v.error();
                }
                builder.CreateRet(*v);
                hasReturn = true;
                break;
            }
            case AstType::expr: {
                auto expr = dynamic_cast<ExpressionAst*>(ast.get());
                auto v = HandleAstNode(*this,*expr->expr);
                if(!v.ok()){
                    return v.error();
                }
            }
            case AstType::assign: {
                auto assign = dynamic_cast<AssignmentAst*>(ast.get());
                auto alloc = namedValues[assign->assignee->id];
                if(!alloc){
                    return std::string("Variable not defined");
                }
                auto v = HandleAstNode(*this,*assign->expr);
                if(!v.ok()){
                    return v.error();
                }
                builder.CreateStore(*v,alloc);
            }
            case AstType::var: {
                auto var = dynamic_cast<VariableDeclareAst*>(ast.get());
                auto v = HandleAstNode(*this, *var->expr);
                if(!v.ok()){
                    return v.error();
                }
                auto alloc = builder.CreateAlloca(llvm::Type::getDoubleTy(file_context->llvm_context()), nullptr,var->identifier);
                namedValues[var->identifier] = alloc;
                builder.CreateStore(*v, alloc);
            }
        }
    }

    if(!hasReturn){
        builder.CreateRetVoid();
    }
    auto verified = llvm::verifyFunction(*generatedFn,&llvm::errs());
    if(!verified){
        return std::string ("Failed lowering function");
    }
    return generatedFn;
}

ErrorOr<llvm::Function *> FunctionContext::HandleDeclaration(CallableDeclaration &declaration) {
    std::vector<llvm::Type*> doubles(function->declaration->args.size(),llvm::Type::getDoubleTy(file_context->llvm_context()));
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(file_context->llvm_context()),doubles,false);
    llvm::Function *F = llvm::Function::Create(FT,llvm::Function::ExternalLinkage,declaration.name,file_context->llvm_module());
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(declaration.args[Idx++]);
    return F;
}

