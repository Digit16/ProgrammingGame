#pragma once

#include "Token.h"
#include <spdlog/spdlog.h>

class BinaryOperation;
class Number;

enum class NodeType : uint8_t { NUMBER = 0, BINARY_OPERATION = 1 };

class AstNode {
public:
  AstNode() = default;
  AstNode(const AstNode &) = default;
  AstNode(AstNode &&) = default;
  AstNode &operator=(const AstNode &) = default;
  AstNode &operator=(AstNode &&) = default;
  virtual ~AstNode() = default;

  virtual void print() const = 0;
  virtual std::string toString() const = 0;

  virtual Token token() const = 0;
  virtual int value() const = 0;

  virtual NodeType nodeType() const = 0;
};

class Number : public AstNode {
public:
  Number(const Token &token) : _token(token) {}

  void print() const override {
    SPDLOG_INFO(std::to_string(_token.getIntegerValue()));
  }

  virtual std::string toString() const override {
    return std::to_string(_token.getIntegerValue());
  }

  Token token() const override { return _token; }
  int value() const override { return _token.getIntegerValue(); }

  NodeType nodeType() const override { return NodeType::NUMBER; }

private:
  Token _token;
};

class BinaryOperation : public AstNode {
public:
  BinaryOperation(const std::shared_ptr<AstNode> &left, const Token &op,
                  const std::shared_ptr<AstNode> &right)
      : _left(left), _operator(op), _right(right) {}

  void print() const override {
    SPDLOG_INFO("({} {} {})", _left->toString(), _operator.toString(),
                _right->toString());
  }
  std::string toString() const override {
    return fmt::format("({} {} {})", _left->toString(), _operator.toString(),
                       _right->toString());
  }

  Token token() const override {
    throw std::runtime_error("Cannot return token from BinaryOperation node");
  }
  int value() const override {
    throw std::runtime_error("Cannot return value from BinaryOperation node");
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

std::variant<Number, BinaryOperation>
getVariant(const std::shared_ptr<AstNode> &node) {
  switch (node->nodeType()) {
  case NodeType::NUMBER: {
    auto number = std::dynamic_pointer_cast<Number>(node);
    if (!number) {
      throw std::runtime_error("Casting failed");
    }

    return std::variant<Number, BinaryOperation>(Number(*number));
  }
  case NodeType::BINARY_OPERATION: {
    auto binOp = std::dynamic_pointer_cast<BinaryOperation>(node);
    if (!binOp) {
      throw std::runtime_error("Casting failed");
    }

    return std::variant<Number, BinaryOperation>(BinaryOperation(*binOp));
  }
  }

  throw std::runtime_error("Unknown NodeType");
}
