#pragma once
#include <vector>
#include <string>

enum class NodeType
{
    program,
    function,
    expression,
    identifier,
    integer,
    list,
};

using StatementList = std::vector<class Statement*>;

class Node {
    public:
        Node() = default;
        virtual ~Node() = default;
        virtual std::string to_string() { return "Node"; };
};

class Expression : public Node {
    public:
        Expression() = default;
        virtual ~Expression() = default;
        std::string to_string() override { return "Expression"; };
};

class Statement : public Node {
    public:
        Statement() = default;
        virtual ~Statement() = default;
        std::string to_string() override { return "Statement"; };
};

class Block : public Expression {
    public:
        StatementList statements;
        Block() = default;
        virtual ~Block() 
        {
            for (auto statement : statements)
            {
                delete statement;
            }
            statements.clear();
        }

        std::string to_string() override { return "Block"; };
};

class Identifier : public Expression {
    public:
        Identifier(std::string name) : name(name) {};
        virtual ~Identifier() = default;
        std::string to_string() override { return name; };
    
    private:
        std::string name;
};

class Integer : public Expression {
    public:
        Integer(int value) : value(value) {};
        virtual ~Integer() = default;
        std::string to_string() override { return std::to_string(value); };
    
    private:
        int value;
};