#pragma once

#include <algorithm>
#include <spdlog/spdlog.h>
#include <vector>

#include "Ast.h"
#include "Lexer.h"
#include "Token.h"

class Parser {
public:
  explicit Parser(const Lexer &lexer)
      : _lexer(lexer), _currentToken(_lexer.getNextToken()) {}

  void raiseInvalidSyntaxError() const {
    throw std::runtime_error("Invalid syntax");
  }

  void eat(TokenType tokenType) {
    if (_currentToken.getType() == tokenType) {
      _currentToken = _lexer.getNextToken();
    } else {
      raiseInvalidSyntaxError();
    }
  }

  // factor : INTEGER | LPAREN expr RPAREN
  std::shared_ptr<AstNode> factor() {
    Token token = _currentToken;
    if (token.getType() == TokenType::INTEGER) {
      eat(TokenType::INTEGER);

      return std::make_shared<Number>(token);
    } else if (token.getType() == TokenType::LPAREN) {
      eat(TokenType::LPAREN);
      std::shared_ptr<AstNode> result = expr();
      eat(TokenType::RPAREN);

      auto binOp = std::dynamic_pointer_cast<BinaryOperation>(result);
      if (!binOp) {
        throw std::runtime_error("Casting failed");
      }
      return std::make_shared<BinaryOperation>(*binOp);
    } else {
      throw std::runtime_error("Unknown token type in factor method");
    }
  }

  // term : factor ((MUL | DIV) factor)*
  std::shared_ptr<AstNode> term() {
    std::shared_ptr<AstNode> node = factor();

    while ((_currentToken.getType() == TokenType::MULTIPLICATION) ||
           (_currentToken.getType() == TokenType::DIVISION)) {

      const Token token = _currentToken;
      if (token.getType() == TokenType::MULTIPLICATION) {
        eat(TokenType::MULTIPLICATION);
      } else if (token.getType() == TokenType::DIVISION) {
        eat(TokenType::DIVISION);
      }

      const auto left = node;
      const auto right = factor();
      node = std::make_shared<BinaryOperation>(left, token, right);
    }

    return node;
  }

  /*
  expr   : term ((PLUS | MINUS) term)*
  term   : factor ((MUL | DIV) factor)*
  factor : INTEGER
  */
  std::shared_ptr<AstNode> expr() {
    std::shared_ptr<AstNode> node = term();

    while ((_currentToken.getType() == TokenType::PLUS) ||
           (_currentToken.getType() == TokenType::MINUS)) {

      const Token token = _currentToken;
      if (token.getType() == TokenType::PLUS) {
        eat(TokenType::PLUS);
      } else if (token.getType() == TokenType::MINUS) {
        eat(TokenType::MINUS);
      }

      const auto left = node;
      const auto right = term();
      node = std::make_shared<BinaryOperation>(left, token, right);
    }

    return node;
  }

  std::shared_ptr<AstNode> parse() { return expr(); }

private:
  Lexer _lexer;
  Token _currentToken;
};
