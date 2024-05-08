#include "FunctionDeclaration.hpp"
#include "AstNode.hpp"
#include "Globals.hpp"
#include "llvm/IR/Value.h"

FunctionDeclaration::FunctionDeclaration(Identifier *name, InstrList *body) : id(name), body(body) {
}

llvm::Value* FunctionDeclaration::codeGen(){
    auto func = llvm::Function::Create(
        llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext), false),
        llvm::Function::CommonLinkage,
        id->to_string(),
        TheModule
    );
    auto insertBlock = llvm::BasicBlock::Create(*TheContext, id->to_string(), func);
    Builder->SetInsertPoint(insertBlock);

    // Generate code for the body
    for (auto instr : *body){
        instr->codeGen();
    }

    return nullptr;
}
