#pragma once
#include "AstNode.hpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#define RED(x) "\033[1;31m" x "\033[0m"

extern char *filename;
extern Block *rootAST; /* root of AST */

/* LLVM OBJECTS */
extern llvm::LLVMContext *TheContext;
extern llvm::IRBuilder<> *Builder;
extern llvm::Module *TheModule;
