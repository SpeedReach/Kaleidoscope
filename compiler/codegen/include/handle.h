//
// Created by gomil on 2023/11/24.
//

#include <llvm/IR/Value.h>
#include "kale_error.h"
#include "ast_node.h"
#include "function_context.h"

static ErrorOr<llvm::Value*> HandleAstNode(FunctionContext& functionContext,AstNode& node);

static ErrorOr<llvm::Value*> HandleNumber(FunctionContext& functionContext,NumberNode& number) {
    return llvm::ConstantFP::get(functionContext.Parent().llvm_context(), llvm::APFloat(number.value));
}

static ErrorOr<llvm::Value*> HandleVariableReference(FunctionContext& functionContext,VariableReferenceNode& ref) {
    auto *V = functionContext.GetValue(ref.id);
    if (!V)
        std::string ("Unknown variable name");
    return functionContext.GetBuilder().CreateLoad(V->getAllocatedType(),V,ref.id.c_str()); // not sure why c_str()
}

static ErrorOr<llvm::Value*> HandleBinaryExpr(FunctionContext& functionContext,BinaryExprNode& expr) {
    auto L = HandleAstNode(functionContext,*expr.lhs);
    if(!L.ok()){
        return L.error();
    }
    auto R = HandleAstNode(functionContext,*expr.rhs);
    if(!R.ok()){
        return R.error();
    }
    switch (expr.op) {
        case BinOperator::add:
            return functionContext.GetBuilder().CreateAdd(*L,*R,"addtmp");
        case BinOperator::subtract:
            return functionContext.GetBuilder().CreateSub(*L,*R,"subtmp");
        case BinOperator::multiply:
            return functionContext.GetBuilder().CreateMul(*L,*R,"multmp");
        case BinOperator::divide:
            return functionContext.GetBuilder().CreateSDiv(*L,*R,"divtmp");
        case BinOperator::greater:
            L = functionContext.GetBuilder().CreateFCmpUGT(*L,*R,"cmptmp");
            return functionContext.GetBuilder().CreateUIToFP(*L,llvm::Type::getDoubleTy(functionContext.Parent().llvm_context()),"booltmp");
        case BinOperator::greaterEqual:
            L = functionContext.GetBuilder().CreateFCmpUGE(*L,*R,"cmptmp");
            return functionContext.GetBuilder().CreateUIToFP(*L,llvm::Type::getDoubleTy(functionContext.Parent().llvm_context()),"booltmp");
        case BinOperator::less:
            L = functionContext.GetBuilder().CreateFCmpULT(*L,*R,"cmptmp");
            return functionContext.GetBuilder().CreateUIToFP(*L,llvm::Type::getDoubleTy(functionContext.Parent().llvm_context()),"booltmp");
        case BinOperator::lessEqual:
            L = functionContext.GetBuilder().CreateFCmpULE(*L,*R,"cmptmp");
            return functionContext.GetBuilder().CreateUIToFP(*L,llvm::Type::getDoubleTy(functionContext.Parent().llvm_context()),"booltmp");
        case BinOperator::equalEqual:
            L = functionContext.GetBuilder().CreateFCmpUEQ(*L,*R,"cmptmp");
            return functionContext.GetBuilder().CreateUIToFP(*L,llvm::Type::getDoubleTy(functionContext.Parent().llvm_context()),"booltmp");
        case BinOperator::notEqual:
            L = functionContext.GetBuilder().CreateFCmpUNE(*L,*R,"cmptmp");
            return functionContext.GetBuilder().CreateUIToFP(*L,llvm::Type::getDoubleTy(functionContext.Parent().llvm_context()),"booltmp");
        case BinOperator::andAnd:
            L = functionContext.GetBuilder().CreateAnd(*L,*R,"andtmp");
            return functionContext.GetBuilder().CreateUIToFP(*L,llvm::Type::getDoubleTy(functionContext.Parent().llvm_context()),"booltmp");
        case BinOperator::orOr:
            L = functionContext.GetBuilder().CreateOr(*L,*R,"ortmp");
            return functionContext.GetBuilder().CreateUIToFP(*L,llvm::Type::getDoubleTy(functionContext.Parent().llvm_context()),"booltmp");
    }
}


static ErrorOr<llvm::Value*> HandleFunctionCall(FunctionContext& functionContext,FunctionCallNode& caller) {
    llvm::Function *CalleeF = functionContext.Parent().llvm_module().getFunction(caller.callee);
    if (!CalleeF)
        return std::string ("Unknown function referenced");

    // If argument mismatch error.
    if (CalleeF->arg_size() != caller.args.size())
        return std::string("Wrong number of arguments");

    std::vector<llvm::Value *> ArgsV;
    for (const auto & arg : caller.args) {
        auto result = HandleAstNode(functionContext,*arg);
        if(!result.ok()){
            return result.error();
        }
        ArgsV.push_back(*result);
    }
    return functionContext.GetBuilder().CreateCall(CalleeF, ArgsV, "calltmp");
}

static ErrorOr<llvm::Value*> HandleAstNode(FunctionContext& functionContext,AstNode& node){
    switch (node.getType()) {
        case AstNodeType::number:
            return HandleNumber(functionContext,dynamic_cast<NumberNode&>(node));
        case AstNodeType::variableReference:
            return HandleVariableReference(functionContext,dynamic_cast<VariableReferenceNode&>(node));
        case AstNodeType::binaryExpr:
            return HandleBinaryExpr(functionContext,dynamic_cast<BinaryExprNode&>(node));
        case AstNodeType::functionCall:
            return HandleFunctionCall(functionContext,dynamic_cast<FunctionCallNode&>(node));
    }
}