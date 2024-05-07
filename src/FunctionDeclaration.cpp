#include "FunctionDeclaration.hpp"
#include "Globals.hpp"

FunctionDeclaration::FunctionDeclaration(Identifier *name, Block*body) : id(name), body(body) {
}

void FunctionDeclaration::codeGen(){
    auto func = llvm::Function::Create(
        llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext), false),
        llvm::Function::ExternalLinkage,
        id->to_string(),
        TheModule
    );
    auto entryBlock = llvm::BasicBlock::Create(*TheContext, id->to_string(), func);
    Builder->SetInsertPoint(entryBlock);

    // Ret Value to 69 for debugging
    auto retValue = llvm::ConstantInt::get(*TheContext, llvm::APInt(32, 69));
    Builder->CreateRet(retValue);
}
