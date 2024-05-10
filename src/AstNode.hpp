#pragma once
#include "ErrorCode.h"
#include "llvm/IR/Constants.h"
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

using StatementList = std::vector<class Statement *>;
using InstrList = std::vector<class Instr *>;

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

class Program : public Node {
public:
  Program(StatementList *statements) : statements(statements) {};
  virtual ~Program() = default;
  StatementList *statements;
  std::string to_string() override { return "Program"; };
  llvm::Value *codeGen() override {
    for (auto statement : *statements) {
      statement->codeGen();
    }
    return nullptr;
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
    Builder->CreateStore(value->codeGen(), a);
    NamedValues->insert(
        std::pair<std::string, llvm::AllocaInst *>(name->to_string(), a));
    return nullptr;
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
    }
    return nullptr;
  }
};
