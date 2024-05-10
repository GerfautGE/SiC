#pragma once
#include "AstNode.hpp"
#include "llvm/IR/Value.h"

class FunctionDeclaration : public Statement {
public:
  explicit FunctionDeclaration(Identifier *name, InstrList *body);
  llvm::Value *codeGen() override;

private:
  Identifier *id;
  InstrList *body;
};
