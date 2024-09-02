#pragma once

#include "Ast.h"
#include "Lexer.h"
#include "Parser.h"

#include <unordered_map>
#include <variant>

class NodeVisitor
{
public:
    struct VisitNode
    {
        // std::shared_ptr<AstNode> operator()(auto& node);
        std::shared_ptr<AstNode> operator()(BinaryOperation& node);
        std::shared_ptr<AstNode> operator()(Number& node);
        std::shared_ptr<AstNode> operator()(UnaryOp& node);
        std::shared_ptr<AstNode> operator()(Section& node); // stop after
        std::shared_ptr<AstNode> operator()(Assign& node); // stop after
        std::shared_ptr<AstNode> operator()(Variable& node);
        std::shared_ptr<AstNode> operator()(EmptyNode& node);
        std::shared_ptr<AstNode> operator()(FunDeclaration& node); // stop after
        std::shared_ptr<AstNode> operator()(FunCall& node); // stop after
        std::shared_ptr<AstNode> operator()(IfStatement& node); // stop after
        std::shared_ptr<AstNode> operator()(WhileLoop& node); // stop after
        std::shared_ptr<AstNode> operator()(ForLoop& node); // stop after
    };

    static std::shared_ptr<AstNode> visit(NodeVariant astNode);

    static int calculateUnaryResult(const std::shared_ptr<AstNode>& node, TokenType op);
    static FlexNumber calculateBinaryResult(const std::shared_ptr<AstNode>& left, const std::shared_ptr<AstNode>& right, TokenType type);
};

class Interpreter : public NodeVisitor
{
public:
    std::shared_ptr<AstNode> interpret(const std::string& text);

    void reset();

    std::map<std::string, std::variant<int, float, bool>>& getGlobalScope() { return GLOBAL_SCOPE; }
    std::unordered_map<std::string, std::vector<std::shared_ptr<AstNode>>>& getGlobalFunctions() { return GLOBAL_FUNCTIONS; }

    void raiseExecutionError(uint16_t currentLine, int16_t currentPositon = -1) const;

    // tutaj raise odpowiedni exception wraz z argumentami w postaci currentLine oraz currentPosition
    // rozwiazac jakos to z ast

private:
    std::map<std::string, std::variant<int, float, bool>> GLOBAL_SCOPE;
    std::unordered_map<std::string, std::vector<std::shared_ptr<AstNode>>> GLOBAL_FUNCTIONS;

    uint16_t _executionPosition;
    uint16_t _executionLine;

    Parser _parser;
};
