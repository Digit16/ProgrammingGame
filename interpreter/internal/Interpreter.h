#pragma once

#include "core/string/ustring.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>


enum class TokenType : uint8_t {
    INTEGER = 1,
    PLUS = 2,
    MINUS = 3,
    MULTIPLICATION = 4,
    DIVISION = 5,
    LPAREN = 6,
    RPAREN = 7,
    END_OF_FILE = 8,
    NONE = 9
};

class Token {
public:
    explicit Token(std::variant<int, char, std::nullptr_t> value, TokenType type) : _value(value), _type(type) {};

    static std::string typeToString(TokenType type);

    std::string toString() const;

    int getIntegerValue() const;

    TokenType getType() const { return _type; }

private:
    std::variant<int, char, std::nullptr_t> _value;
    TokenType _type;
};

class BinaryOperation;
class Number;

enum class NodeType : uint8_t {
    NUMBER = 0,
    BINARY_OPERATION = 1
};

class AstNode {
public:
    AstNode() = default;
    AstNode(const AstNode&) = default;
    AstNode(AstNode&&) = default;
    AstNode& operator=(const AstNode&) = default;
    AstNode& operator=(AstNode&&) = default;
    virtual ~AstNode() = default;

    virtual void print() const = 0;
    virtual std::string toString() const = 0;

    virtual Token token() const = 0;
    virtual int value() const = 0;

    virtual NodeType nodeType() const = 0;
};

class Number : public AstNode {
public:
    Number(const Token& token) : _token(token) {}

    void print() const override {
        std::cout << std::to_string(_token.getIntegerValue()) << std::endl;
    }

    virtual std::string toString() const override {
        return std::to_string(_token.getIntegerValue());
    }

    Token token() const override {
        return _token;
    }
    int value() const override {
        return _token.getIntegerValue();
    }

    NodeType nodeType() const override {
        return NodeType::NUMBER;
    }

private:
    Token _token;
};

class BinaryOperation : public AstNode {
public:
    BinaryOperation(const std::shared_ptr<AstNode>& left, const Token& op, const std::shared_ptr<AstNode>& right) :
    _left(left),
    _operator(op),
    _right(right) {}

    void print() const override {
        std::cout << "(" << _left->toString() << " " << _operator.toString() << " " << _right->toString() << ")" << std::endl;
    }
    std::string toString() const override {
        return std::string("(" + _left->toString() + " " + _operator.toString() + " " + _right->toString() + ")");
    }

    Token token() const override {
        throw std::runtime_error("Cannot return token from BinaryOperation node");
    }
    int value() const override {
        throw std::runtime_error("Cannot return value from BinaryOperation node");
    }

    std::shared_ptr<AstNode> left() const {
        return _left;
    }
    Token binaryOperator() const {
        return _operator;
    }
    std::shared_ptr<AstNode> right() const {
        return _right;
    }

    NodeType nodeType() const override {
        return NodeType::BINARY_OPERATION;
    }

private:
    std::shared_ptr<AstNode> _left;
    Token _operator;
    std::shared_ptr<AstNode> _right;
};

class NodeConverter {
public:
    static std::variant<Number, BinaryOperation> getVariant(const std::shared_ptr<AstNode>& node);
};

class Lexer {
public:
    Lexer(const std::string& text) : _text(text), _pos(0), _currentChar(_text.at(_pos)) {}

    void raiseInvalidCharacterError() const { throw std::runtime_error("Invalid character"); }
    
    void advance();
    
    void skipWhitespace();
    
    int integer();

    Token getNextToken();

private:
    std::string _text;
    int _pos;
    char _currentChar;
};

class Parser {
public:
    explicit Parser(const Lexer& lexer) : _lexer(lexer), _currentToken(_lexer.getNextToken()) {}

    void raiseInvalidSyntaxError() const { throw std::runtime_error("Invalid syntax"); }

    void eat(TokenType tokenType);

    std::shared_ptr<AstNode> factor();

    std::shared_ptr<AstNode> term();

    std::shared_ptr<AstNode> expr();

    std::shared_ptr<AstNode> parse() { return expr(); }

private:
    Lexer _lexer;
    Token _currentToken;
};

class NodeVisitor {
public:
    struct VisitNode
    {
        // std::shared_ptr<AstNode> operator()(auto& node);
        std::shared_ptr<AstNode> operator()(BinaryOperation& node);
        std::shared_ptr<AstNode> operator()(Number& node);
    };

    static std::shared_ptr<AstNode> visit(std::variant<Number, BinaryOperation> astNode);

    static int calculateResult(const std::shared_ptr<AstNode>& left, const std::shared_ptr<AstNode>& right, TokenType type);
};

class Interpreter : public NodeVisitor
{
public:
    Interpreter() {}
    // Interpreter(const std::string& text) : _lexer(text), _parser(_lexer) {}        

    std::shared_ptr<AstNode> interpret(const std::string& text);
};

