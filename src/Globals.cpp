#include "Globals.hpp"
#include "AstNode.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

// file name of the input file
char *filename = nullptr;

// AST
Node *rootAST = nullptr;

/* LLVM OBJECTS */
llvm::LLVMContext *TheContext = nullptr;
llvm::IRBuilder<> *Builder = nullptr;
llvm::Module *TheModule = nullptr;
std::map<std::string, llvm::AllocaInst *> *NamedValues;
