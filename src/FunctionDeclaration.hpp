#pragma once
#include "AstNode.hpp"

class FunctionDeclaration : public Statement {
    public:
        explicit FunctionDeclaration(Identifier *name, Block *body);
        void codeGen();

    private:
        Identifier *id;
        Block *body;
};
