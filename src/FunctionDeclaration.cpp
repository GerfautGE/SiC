#include "FunctionDeclaration.hpp"
#include "AstNode.hpp"
#include "Globals.hpp"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

FunctionDeclaration::FunctionDeclaration(Identifier *name, InstrList *body) : id(name), body(body) {
}

llvm::Value* FunctionDeclaration::codeGen(){
    std::cout << "Generating code for function declaration " << id->to_string() << std::endl;


    TheFunction = llvm::Function::Create(
        llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext),  false),
        llvm::Function::CommonLinkage,
        id->to_string(),
        TheModule
    );
    auto insertBlock = llvm::BasicBlock::Create(*TheContext, id->to_string(), TheFunction);
    Builder->SetInsertPoint(insertBlock);

    // Clear the symbol table
    NamedValues->clear();

    // Generate code for the body
    int i = 0;
    for (auto instr : *body){
        std::cout << "Generating code for instruction " << i++ << std::endl;
        instr->codeGen();
    }

    llvm::verifyFunction(*TheFunction);

    return TheFunction;
}
