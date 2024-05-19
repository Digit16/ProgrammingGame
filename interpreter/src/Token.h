#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <variant>

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
  explicit Token(std::variant<int, char, std::nullptr_t> value, TokenType type)
      : _value(value), _type(type){};

  static std::string typeToString(TokenType type) {
    switch (type) {
    case TokenType::INTEGER:
      return "INTEGER";
    case TokenType::PLUS:
      return "PLUS";
    case TokenType::MINUS:
      return "MINUS";
    case TokenType::MULTIPLICATION:
      return "MULTIPLICATION";
    case TokenType::DIVISION:
      return "DIVISION";
    case TokenType::LPAREN:
      return "LPAREN";
    case TokenType::RPAREN:
      return "RPAREN";
    case TokenType::END_OF_FILE:
      return "END_OF_FILE";
    case TokenType::NONE:
      return "NONE";
    }

    throw std::runtime_error(
        "TokenType not implemented, string conversion is not possible");
  }

  std::string toString() const {
    if (std::holds_alternative<int>(_value)) {
      return std::format("TOKEN({},{})", typeToString(_type),
                         std::to_string(std::get<int>(_value)));
    } else if (std::holds_alternative<char>(_value)) {
      return std::format("TOKEN({},{})", typeToString(_type),
                         std::get<char>(_value));
    } else if (std::holds_alternative<std::nullptr_t>(_value)) {
      return std::format("TOKEN({},{})", typeToString(_type), "NONE");
    } else {
      throw std::runtime_error(
          std::format("Variant holds unsupported type '{}", _value.index()));
    }
  }

  TokenType getType() const { return _type; }

  int getIntegerValue() const {
    if (std::holds_alternative<int>(_value)) {
      return std::get<int>(_value);
    }

    throw std::runtime_error(
        "Variant does not hold integer inside, cannot return value of it");
  }

private:
  std::variant<int, char, std::nullptr_t> _value;
  TokenType _type;
};
