#pragma once


#include "Ast.h"
#include "Lexer.h"
#include "Parser.h"

#include <variant>

class NodeVisitor
{
public:
    struct VisitNode
    {
        //std::shared_ptr<AstNode> operator()(auto& node);
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
};
