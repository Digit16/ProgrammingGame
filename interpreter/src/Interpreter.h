#pragma once

#include "Ast.h"
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"

#include <variant>

class NodeVisitor {
public:
  struct VisitNode {
    std::shared_ptr<AstNode> operator()(auto &node) {
      throw std::runtime_error("Unsupported AstNode in NodeVisitor");
    }
    std::shared_ptr<AstNode> operator()(BinaryOperation &node) {

      std::variant<Number, BinaryOperation> left = getVariant(node.left());
      std::variant<Number, BinaryOperation> right = getVariant(node.right());

      TokenType tokenType = node.binaryOperator().getType();
      if (tokenType == TokenType::PLUS) {
        int value = calculateResult(visit(left), visit(right), tokenType);
        return std::make_shared<Number>(Token(value, TokenType::INTEGER));
      } else if (tokenType == TokenType::MINUS) {
        int value = calculateResult(visit(left), visit(right), tokenType);
        return std::make_shared<Number>(Token(value, TokenType::INTEGER));
      } else if (tokenType == TokenType::MULTIPLICATION) {
        int value = calculateResult(visit(left), visit(right), tokenType);
        return std::make_shared<Number>(Token(value, TokenType::INTEGER));
      } else if (tokenType == TokenType::DIVISION) {
        int value = calculateResult(visit(left), visit(right), tokenType);
        return std::make_shared<Number>(Token(value, TokenType::INTEGER));
      } else {
        throw std::runtime_error("Unknown type of binary operator in visitor");
      }
    }
    std::shared_ptr<AstNode> operator()(Number &node) {
      return std::make_shared<Number>(Token(node.value(), TokenType::INTEGER));
    }
  };

  static std::shared_ptr<AstNode>
  visit(std::variant<Number, BinaryOperation> astNode) {
    return std::visit(VisitNode(), astNode);
  }

  static int calculateResult(const std::shared_ptr<AstNode> &left,
                             const std::shared_ptr<AstNode> &right,
                             TokenType type) {
    auto leftNumber = std::dynamic_pointer_cast<Number>(left);
    auto rightNumber = std::dynamic_pointer_cast<Number>(right);

    if ((!leftNumber) || (!rightNumber)) {
      throw std::runtime_error("Casting failed");
    }

    int leftValue = leftNumber->value();
    int rightValue = rightNumber->value();

    if (type == TokenType::PLUS) {
      return leftValue + rightValue;
    } else if (type == TokenType::MINUS) {
      return leftValue - rightValue;
    } else if (type == TokenType::MULTIPLICATION) {
      return leftValue * rightValue;
    } else if (type == TokenType::DIVISION) {
      return leftValue / rightValue;
    } else {
      throw std::runtime_error("Unknown token type in calculateResult");
    }
  }
};

class Interpreter : public NodeVisitor {
public:
  Interpreter(const Parser &parser) : _parser(parser) {}

  std::shared_ptr<AstNode> interpret() {
    std::shared_ptr<AstNode> tree = _parser.parse();
    return visit(getVariant(tree));
  }

private:
  Parser _parser;
};
