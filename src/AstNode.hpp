#pragma once
#include "ErrorCode.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

extern llvm::LLVMContext *TheContext;
extern llvm::IRBuilder<> *Builder;
extern llvm::Module *TheModule;
extern std::map<std::string, llvm::AllocaInst *> *NamedValues;

/**
 * Base class for all nodes in the AST
 */
class Node {
public:
  Node() = default;
  virtual ~Node() = default;
  virtual std::string to_string() { return "Node"; };
  virtual llvm::Value *codeGen() {
    std::cout << "Node codeGen" << std::endl;
    return nullptr;
  }
};

/**
 * Base class for all expressions in the AST
 */
class Expression : public Node {
public:
  Expression() = default;
  virtual ~Expression() = default;
  std::string to_string() override { return "Expression"; };
  virtual llvm::Value *codeGen() override {
    std::cout << "Expression codeGen" << std::endl;
    return nullptr;
  }
};

class Instr : public Node {
public:
  Instr() = default;
  virtual ~Instr() = default;
  std::string to_string() override { return "Instruction"; };
  virtual llvm::Value *codeGen() override {
    std::cout << "Instruction codeGen" << std::endl;
    return nullptr;
  }
};

class Statement : public Node {
public:
  Statement() = default;
  virtual ~Statement() = default;
  std::string to_string() override { return "Statement"; };
  virtual llvm::Value *codeGen() override {
    std::cout << "Statement codeGen" << std::endl;
    return nullptr;
  }
};

class StatementList : public std::vector<Statement *> {
public:
  StatementList() = default;
  virtual ~StatementList() = default;
  std::string to_string() { return "StatementList"; };
  llvm::Value *codeGen() {
    llvm::Value *last = nullptr;
    for (auto statement : *this) {
      last = statement->codeGen();
    }
    return last;
  }
};

class InstrList : public std::vector<Instr *> {
public:
  InstrList() = default;
  virtual ~InstrList() = default;
  std::string to_string() { return "InstrList"; };
  llvm::Value *codeGen() {
    llvm::Value *last = nullptr;
    for (auto instr : *this) {
      last = instr->codeGen();
    }
    return last;
  }
};

class Program : public Node {
public:
  Program(StatementList *statements) : statements(statements) {};
  virtual ~Program() = default;
  StatementList *statements;
  std::string to_string() override { return "Program"; };
  llvm::Value *codeGen() override {
    llvm::Value *last = nullptr;
    for (auto statement : *statements) {
      last = statement->codeGen();
    }
    return last;
  }
};

class Identifier : public Expression {
public:
  Identifier(std::string name) : name(name) {};
  virtual ~Identifier() = default;
  std::string to_string() override { return name; };
  virtual llvm::Value *codeGen() override {
    // lookup in NamedValues
    llvm::AllocaInst *a = NamedValues->at(name);
    if (!a) {
      std::cerr << "Unknown variable name " << name << std::endl;
      exit(ERROR_CODE::UNKNOWN_VARIABLE_ERROR);
    }
    return Builder->CreateLoad(a->getAllocatedType(), a, name.c_str());
  }

private:
  std::string name;
};

class Integer : public Expression {
public:
  Integer(int value) : value(value) {};
  virtual ~Integer() = default;
  std::string to_string() override { return std::to_string(value); };
  llvm::Value *codeGen() override {
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*TheContext), value,
                                  true);
  }

private:
  int value;
};

class Return_Instr : public Instr {
public:
  Return_Instr(Expression *return_value) : return_value(return_value) {};
  Expression *return_value;
  llvm::Value *codeGen() override {
    llvm::Value *ret = return_value->codeGen();
    Builder->CreateRet(ret);
    return ret;
  }
};

class Decl_Instr : public Instr {
public:
  Decl_Instr(Identifier *name, Expression *value) : name(name), value(value) {};
  Identifier *name;
  Expression *value;
  llvm::Value *codeGen() override {
    llvm::AllocaInst *a = Builder->CreateAlloca(
        llvm::Type::getInt32Ty(*TheContext), nullptr, name->to_string());
    llvm::Value *v = value->codeGen();
    Builder->CreateStore(v, a);
    NamedValues->insert(
        std::pair<std::string, llvm::AllocaInst *>(name->to_string(), a));
    return v;
  }
};

class Call_Instr : public Instr {
public:
  Call_Instr(Identifier *name) : name(name) {};
  Identifier *name;
  std::vector<Expression *> *args;
  llvm::Value *codeGen() override {
    llvm::Function *callee = TheModule->getFunction(name->to_string());
    if (!callee) {
      std::cerr << "Unknown function referenced" << std::endl;
      exit(ERROR_CODE::UNKNOWN_FUNCTION_ERROR);
    }
    return Builder->CreateCall(callee);
  }
};

class Call_Expr : public Expression {
public:
  Call_Expr(Identifier *name) : name(name) {};
  Identifier *name;
  llvm::Value *codeGen() override {
    // Look up the name in the global module table.
    llvm::Function *CalleeF = TheModule->getFunction(name->to_string());
    if (!CalleeF) {
      std::cerr << "Unknown function: " << name->to_string() << std::endl;
      exit(ERROR_CODE::UNKNOWN_FUNCTION_ERROR);
    }
    return Builder->CreateCall(CalleeF);
  }
};

enum class Binop {
  Plus,
  Minus,
  Times,
  Divide,
  Modulo,
  Lsr,
  Lsl,
  Equal,
  NotEqual,
};

class Binop_Expr : public Expression {
public:
  Binop_Expr(Expression *lhs, Expression *rhs, Binop op)
      : lhs(lhs), rhs(rhs), op(op) {};
  Expression *lhs;
  Expression *rhs;
  Binop op;

  llvm::Value *codeGen() override {
    llvm::Value *l = lhs->codeGen();
    llvm::Value *r = rhs->codeGen();
    switch (op) {
    case Binop::Plus:
      return Builder->CreateAdd(l, r, "addtmp");
    case Binop::Minus:
      return Builder->CreateSub(l, r, "subtmp");
    case Binop::Times:
      return Builder->CreateMul(l, r, "multmp");
    case Binop::Divide:
      return Builder->CreateSDiv(l, r, "divtmp");
    case Binop::Modulo:
      return Builder->CreateSRem(l, r, "modtmp");
    case Binop::Lsr:
      return Builder->CreateLShr(l, r, "lsrtmp");
    case Binop::Lsl:
      return Builder->CreateShl(l, r, "lsrtmp");
    case Binop::Equal:
      return Builder->CreateICmpEQ(l, r, "eqtmp");
    case Binop::NotEqual:
      return Builder->CreateICmpNE(l, r, "neqtmp");
    }
  }
};

class If_Instr : public Instr {
public:
  Expression *Cond;
  InstrList *Then;
  InstrList *Else;
  If_Instr(Expression *cond, InstrList *then_instr, InstrList *else_instr)
      : Cond(cond), Then(then_instr), Else(else_instr) {}

  llvm::Value *codeGen() override {
    llvm::Value *CondV = Cond->codeGen();
    if (!CondV)
      return nullptr;

    // Check if the condition is not boolean
    if (!CondV->getType()->isIntegerTy(1)) {
      // Convert condition to a bool by comparing non-equal to 0.
      CondV = Builder->CreateICmpNE(
          CondV, llvm::ConstantInt::get(*TheContext, llvm::APInt(32, 0)),
          "ifcond");
    }
    llvm::Function *aFunction = Builder->GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at
    // the end of the function.
    llvm::BasicBlock *ThenBB =
        llvm::BasicBlock::Create(*TheContext, "then", aFunction);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(*TheContext, "else");
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*TheContext, "ifcont");

    Builder->CreateCondBr(CondV, ThenBB, ElseBB);

    // Emit then value.
    Builder->SetInsertPoint(ThenBB);

    llvm::Value *ThenV = Then->codeGen();

    Builder->CreateBr(MergeBB);
    // Codegen of 'Then' can change the current block, update ThenBB for the
    // PHI.
    ThenBB = Builder->GetInsertBlock();

    // Emit else block.
    aFunction->insert(aFunction->end(), ElseBB);
    Builder->SetInsertPoint(ElseBB);

    llvm::Value *ElseV = Else->codeGen();

    Builder->CreateBr(MergeBB);
    // Codegen of 'Else' can change the current block, update ElseBB for the
    // PHI.
    ElseBB = Builder->GetInsertBlock();

    // Emit merge block.
    aFunction->insert(aFunction->end(), MergeBB);
    Builder->SetInsertPoint(MergeBB);
    llvm::PHINode *PN =
        Builder->CreatePHI(llvm::Type::getInt32Ty(*TheContext), 2, "iftmp");
    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);

    return PN;
  };
};
