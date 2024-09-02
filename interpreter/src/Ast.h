#pragma once

#include "Token.h"

//#include <spdlog/spdlog.h>
#include <memory>
#include <vector>

class AstNode;
class Number;
class BinaryOperation;
class UnaryOp;
class Section;
class Assign;
class Variable;
class EmptyNode;
class FunDeclaration;
class FunCall;
class IfStatement;
class WhileLoop;
class ForLoop;

using NodeVariant =
    std::variant<Number, BinaryOperation, UnaryOp, Section, Assign, Variable, EmptyNode, FunDeclaration, FunCall, IfStatement, WhileLoop, ForLoop>;

using FlexNumber = std::variant<int, float, bool>;

enum class NodeType : uint8_t
{
    NUMBER = 0,
    BINARY_OPERATION = 1,
    UNARY_OPERATOR = 2,
    SECTION = 3,
    ASSIGN = 4,
    VARIABLE = 5,
    EMPTY = 6,
    FUN_DECLARATION = 7,
    FUN_CALL = 8,
    IF = 9,
    WHILE = 10,
    FOR = 11
};

class Token;

class AstNode
{
public:
    AstNode() = default;
    AstNode(const AstNode&) = default;
    AstNode(AstNode&&) = default;
    AstNode& operator=(const AstNode&) = default;
    AstNode& operator=(AstNode&&) = default;
    virtual ~AstNode() = default;

    virtual NodeType nodeType() const = 0;
};

class Number : public AstNode // Can be bool
{
public:
    Number(const Token& token) :
        _token(token)
    {
    }

    Token token() const { return _token; }
    FlexNumber value() const { return _token.getFlexNumber(); }

    NodeType nodeType() const override { return NodeType::NUMBER; }

private:
    Token _token;
};

class BinaryOperation : public AstNode
{
public:
    BinaryOperation(const std::shared_ptr<AstNode>& left, const Token& op, const std::shared_ptr<AstNode>& right) :
        _left(left),
        _operator(op),
        _right(right)
    {
    }

    std::shared_ptr<AstNode> left() const { return _left; }
    Token binaryOperator() const { return _operator; }
    std::shared_ptr<AstNode> right() const { return _right; }

    NodeType nodeType() const override { return NodeType::BINARY_OPERATION; }

private:
    std::shared_ptr<AstNode> _left;
    Token _operator;
    std::shared_ptr<AstNode> _right;
};

class UnaryOp : public AstNode
{
public:
    UnaryOp(const Token& op, const std::shared_ptr<AstNode>& expr) :
        _op(op),
        _expr(expr)
    {
    }

    Token token() const { return _op; }

    std::shared_ptr<AstNode> expr() const { return _expr; }

    NodeType nodeType() const override { return NodeType::UNARY_OPERATOR; }

private:
    Token _op;
    std::shared_ptr<AstNode> _expr;
};

class Section : public AstNode
{
public:
    Section(const std::vector<std::shared_ptr<AstNode>>& children) :
        _children(children)
    {
    }

    std::vector<std::shared_ptr<AstNode>> children() const { return _children; }

    NodeType nodeType() const override { return NodeType::SECTION; }

private:
    std::vector<std::shared_ptr<AstNode>> _children;
};

class Assign : public AstNode
{
public:
    Assign(std::shared_ptr<AstNode> left, const Token& op, std::shared_ptr<AstNode> right) :
        _left(left),
        _op(op),
        _right(right)
    {
    }

    Token token() const { return _op; }

    std::shared_ptr<AstNode> left() const { return _left; }
    std::shared_ptr<AstNode> right() const { return _right; }

    NodeType nodeType() const override { return NodeType::ASSIGN; }

private:
    std::shared_ptr<AstNode> _left;
    Token _op;
    std::shared_ptr<AstNode> _right;
};

class Variable : public AstNode
{
public:
    Variable(const Token& token) :
        _token(token),
        _name(token.getStringValue())
    {
    }

    Token token() const { return _token; }

    std::string name() const { return _name; }

    NodeType nodeType() const override { return NodeType::VARIABLE; }

private:
    Token _token;
    std::string _name;
};

class EmptyNode : public AstNode
{
public:
    NodeType nodeType() const override { return NodeType::EMPTY; }
};

class FunDeclaration : public AstNode
{
public:
    FunDeclaration(const std::string& name, const std::vector<std::shared_ptr<AstNode>>& body) :
        _name(name),
        _body(body)
    {
    }

    const std::string& name() const { return _name; }
    const std::vector<std::shared_ptr<AstNode>>& body() const { return _body; }

    NodeType nodeType() const override { return NodeType::FUN_DECLARATION; }

private:
    std::string _name;
    std::vector<std::shared_ptr<AstNode>> _body;
};

class FunCall : public AstNode
{
public:
    FunCall(const std::string& name) :
        _name(name)
    {
    }

    const std::string& name() const { return _name; }

    NodeType nodeType() const override { return NodeType::FUN_CALL; }

private:
    std::string _name;
};

class IfStatement : public AstNode
{
public:
    IfStatement(std::shared_ptr<AstNode> condition, std::shared_ptr<AstNode> thenBranch, std::shared_ptr<AstNode> elseBranch) :
        _condition(condition),
        _thenBranch(thenBranch),
        _elseBranch(elseBranch)
    {
    }

    NodeType nodeType() const override { return NodeType::IF; }

    std::shared_ptr<AstNode> condition() const { return _condition; }
    std::shared_ptr<AstNode> thenBranch() const { return _thenBranch; }
    std::shared_ptr<AstNode> elseBranch() const { return _elseBranch; }

private:
    std::shared_ptr<AstNode> _condition;
    std::shared_ptr<AstNode> _thenBranch;
    std::shared_ptr<AstNode> _elseBranch;
};

class WhileLoop : public AstNode
{
public:
    WhileLoop(std::shared_ptr<AstNode> condition, std::shared_ptr<AstNode> body) :
        _condition(condition),
        _body(body)
    {
    }

    NodeType nodeType() const override { return NodeType::WHILE; }

    std::shared_ptr<AstNode> condition() const { return _condition; }
    std::shared_ptr<AstNode> body() const { return _body; }

private:
    std::shared_ptr<AstNode> _condition;
    std::shared_ptr<AstNode> _body;
};

class ForLoop : public AstNode
{
public:
    ForLoop(std::shared_ptr<AstNode> initialization,
            std::shared_ptr<AstNode> condition,
            std::shared_ptr<AstNode> increment,
            std::shared_ptr<AstNode> body) :
        _initialization(initialization),
        _condition(condition),
        _increment(increment),
        _body(body)
    {
    }

    NodeType nodeType() const override { return NodeType::FOR; }

    std::shared_ptr<AstNode> initialization() const { return _initialization; }
    std::shared_ptr<AstNode> condition() const { return _condition; }
    std::shared_ptr<AstNode> increment() const { return _increment; }
    std::shared_ptr<AstNode> body() const { return _body; }

private:
    std::shared_ptr<AstNode> _initialization;
    std::shared_ptr<AstNode> _condition;
    std::shared_ptr<AstNode> _increment;
    std::shared_ptr<AstNode> _body;
};

NodeVariant getVariant(const std::shared_ptr<AstNode>& node);

std::string getTypeString(NodeType nt);

struct FlexNumberAdder
{
    template <typename T, typename U>
    auto operator()(T lhs, U rhs) const
    {
        return FlexNumber(lhs + rhs);
    }
};

struct FlexNumberSubtractor
{
    template <typename T, typename U>
    auto operator()(T lhs, U rhs) const
    {
        return FlexNumber(lhs - rhs);
    }
};

struct FlexNumberMultiplier
{
    template <typename T, typename U>
    auto operator()(T lhs, U rhs) const
    {
        return FlexNumber(lhs * rhs);
    }
};

struct FlexNumberDivider
{
    template <typename T, typename U>
    auto operator()(T lhs, U rhs) const
    {
        if (rhs == 0) {
            throw std::runtime_error("Division by zero");
        }
        return FlexNumber(lhs / rhs);
    }
};

FlexNumber operator+(const FlexNumber& lhs, const FlexNumber& rhs);

FlexNumber operator-(const FlexNumber& lhs, const FlexNumber& rhs);

FlexNumber operator*(const FlexNumber& lhs, const FlexNumber& rhs);

FlexNumber operator/(const FlexNumber& lhs, const FlexNumber& rhs);
