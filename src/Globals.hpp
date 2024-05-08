#pragma once
#include "AstNode.hpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include <map>

#define RED(x) "\033[1;31m" x "\033[0m"

extern char *filename;
extern Node *rootAST; /* root of AST */

/* LLVM OBJECTS */
extern llvm::LLVMContext *TheContext;
extern llvm::IRBuilder<> *Builder;
extern llvm::Module *TheModule;
extern std::map<std::string, llvm::AllocaInst*> *NamedValues;
