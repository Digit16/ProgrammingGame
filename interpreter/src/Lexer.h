#pragma once

#include "Token.h"

class Lexer {
public:
  Lexer(const std::string &text)
      : _text(text), _pos(0), _currentChar(_text.at(_pos)) {}

  void raiseInvalidCharacterError() const {
    throw std::runtime_error("Invalid character");
  }

  void advance() {
    ++_pos;
    if (_pos > static_cast<int>(_text.length()) - 1) {
      _currentChar = '\0';
    } else {
      _currentChar = _text[_pos];
    }
  }

  void skipWhitespace() {
    while (_currentChar != '\0' && _currentChar == ' ') {
      advance();
    }
  }

  int integer() {
    std::string result;
    while (_currentChar != '\0' && isdigit(_currentChar)) {
      result += _currentChar;
      advance();
    }

    return std::stoi(result);
  }

  Token getNextToken() {
    while (_currentChar != '\0') {

      if (_currentChar == ' ') {
        skipWhitespace();
        continue;
      }

      if (isdigit(_currentChar)) {
        return Token(integer(), TokenType::INTEGER);
      }

      if (_currentChar == '+') {
        advance();
        return Token('+', TokenType::PLUS);
      }

      if (_currentChar == '-') {
        advance();
        return Token('-', TokenType::MINUS);
      }

      if (_currentChar == '*') {
        advance();
        return Token('*', TokenType::MULTIPLICATION);
      }

      if (_currentChar == '/') {
        advance();
        return Token('/', TokenType::DIVISION);
      }

      if (_currentChar == '(') {
        advance();
        return Token('(', TokenType::LPAREN);
      }

      if (_currentChar == ')') {
        advance();
        return Token(')', TokenType::RPAREN);
      }

      raiseInvalidCharacterError();
    }

    return Token(std::nullptr_t(), TokenType::END_OF_FILE);
  }

private:
  std::string _text;
  int _pos;
  char _currentChar;
};
