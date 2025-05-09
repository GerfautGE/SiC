#include "FunctionDeclaration.hpp"
#include "AstNode.hpp"
#include "Globals.hpp"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

FunctionDeclaration::FunctionDeclaration(Identifier *name, InstrList *body)
    : id(name), body(body) {}

llvm::Value *FunctionDeclaration::codeGen() {
  // set the linkage to external if the function is main
  llvm::GlobalValue::LinkageTypes linkage;
  if (id->to_string() == "main") {
    linkage = llvm::Function::ExternalLinkage;
  } else {
    linkage = llvm::Function::InternalLinkage;
  }
  TheFunction = llvm::Function::Create(
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext), false),
      linkage, id->to_string(), TheModule);
  auto insertBlock =
      llvm::BasicBlock::Create(*TheContext, id->to_string(), TheFunction);
  Builder->SetInsertPoint(insertBlock);

  // Clear the symbol table
  NamedValues->clear();

  // Generate code for the body
  for (auto instr : *body) {
    instr->codeGen();
  }

  llvm::verifyFunction(*TheFunction);

  return TheFunction;
}
