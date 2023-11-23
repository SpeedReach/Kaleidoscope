//
// Created by gomil on 2023/11/22.
//

#ifndef KALEIDOSCOPE_FILE_CONTEXT_H
#define KALEIDOSCOPE_FILE_CONTEXT_H

#include <vector>
#include <memory>
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"

class FileContext {
public:
    auto llvm_context() -> llvm::LLVMContext& { return *llvm_context_; }
    auto llvm_module() -> llvm::Module& { return *llvm_module_; }

    FileContext():
    llvm_context_(std::make_unique<llvm::LLVMContext>())
    {
        llvm_module_ = std::make_unique<llvm::Module>("test",*llvm_context_);
    }
private:
    std::unique_ptr<llvm::LLVMContext> llvm_context_;
    std::unique_ptr<llvm::Module> llvm_module_;

};


#endif //KALEIDOSCOPE_FILE_CONTEXT_H
