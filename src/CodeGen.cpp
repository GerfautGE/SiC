#include "CodeGen.hpp"
#include "ErrorCode.hpp"
#include "Globals.hpp"
#include "llvm/IR/Instructions.h"
#include <iostream>

void setupCodeGen() {
  // Open a new context and module.
  TheContext = new llvm::LLVMContext();
  if (!TheContext) {
    std::cerr << "Error: Could not create LLVM context." << std::endl;
    exit(ERROR_CODE::OTHER_ERROR);
  }

  TheModule = new llvm::Module("SiC", *TheContext);
  if (!TheModule) {
    std::cerr << "Error: Could not create LLVM module." << std::endl;
    exit(ERROR_CODE::OTHER_ERROR);
  }

  // Create a new builder for the module.
  Builder = new llvm::IRBuilder<>(*TheContext);
  if (!Builder) {
    std::cerr << "Error: Could not create IR builder." << std::endl;
    exit(ERROR_CODE::OTHER_ERROR);
  }
  // Create the NamedValues map.
  NamedValues = new std::map<std::string, llvm::AllocaInst *>();
}

void set_entryPoint() {
  // Create the entry function.
  auto mainFunc = llvm::Function::Create(
      llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext), false),
      llvm::Function::ExternalLinkage, "entry", TheModule);

  // Create the entry block.
  auto entryBlock = llvm::BasicBlock::Create(*TheContext, "entry", mainFunc);
  Builder->SetInsertPoint(entryBlock);

  // Call "main" function
  auto mainFuncPtr = TheModule->getFunction("main");
  if (!mainFuncPtr) {
    std::cerr << "Error: main function not found." << std::endl;
    exit(ERROR_CODE::NO_MAIN_ERROR);
  }

  // Icall main function. Ensure that the main function exists.
  Builder->CreateCall(mainFuncPtr);
  Builder->CreateRetVoid();
}

void codeGen() {
  // assert rootAST is not null
  if (!rootAST) {
    std::cerr << "Error: Root AST node is null." << std::endl;
    exit(ERROR_CODE::EMPTY_AST_ERROR);
  }

  rootAST->codeGen();
}
