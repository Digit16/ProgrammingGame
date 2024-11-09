#include "Token.h"

// static
std::string Token::typeToString(TokenType type)
{
    switch (type) {
    case TokenType::INTEGER:
        return "INTEGER";
    case TokenType::FLOATING_NUMBER:
        return "FLOATING_NUMBER";
    case TokenType::BOOL_VALUE:
        return "BOOL_VALUE";
    case TokenType::COLON:
        return "COLON";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::MULTIPLICATION:
        return "MULTIPLICATION";
    case TokenType::DIVISION:
        return "DIVISION";
    case TokenType::LPAREN:
        return "LEFT_PARENTHESIS";
    case TokenType::RPAREN:
        return "RIGHT_PARENTHESIS";
    case TokenType::END_OF_FILE:
        return "END_OF_FILE";
    case TokenType::NONE:
        return "NONE";
    case TokenType::ID:
        return "ID";
    case TokenType::DOT:
        return "DOT";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::ASSIGN:
        return "ASSIGN";
    case TokenType::START:
        return "START";
    case TokenType::END:
        return "END";
    case TokenType::IF:
        return "IF";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::FOR:
        return "FOR";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::COMPARISON:
        return "COMPARISON";
    case TokenType::NOT_EQUAL:
        return "NOT_EQUAL";
    case TokenType::GREATER:
        return "GREATER";
    case TokenType::LESS:
        return "LESS";
    case TokenType::GREATER_EQUAL:
        return "GREATER_EQUAL";
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
    case TokenType::NEWLINE:
        return "NEWLINE";
    case TokenType::VARIABLE_DECLARATION:
        return "VARIABLE_DECLARATION";
    case TokenType::FUN_DECLARATION:
        return "FUN_DECLARATION";
    }

    throw std::runtime_error("TokenType not implemented, string conversion is not possible");
}

std::string Token::toDebugString() const
{
    std::stringstream ss;

    if (std::holds_alternative<int>(_value)) {
        ss << "TOKEN(" << typeToString(_type) << "," << std::to_string(std::get<int>(_value)) << ")";
    } else if (std::holds_alternative<char>(_value)) {
        ss << "TOKEN(" << typeToString(_type) << "," << std::get<char>(_value) << ")";
    } else if (std::holds_alternative<std::nullptr_t>(_value)) {
        ss << "TOKEN(" << typeToString(_type) << ",NONE)";
    } else if (std::holds_alternative<std::string>(_value)) {
        ss << "TOKEN(" << typeToString(_type) << "," << std::get<std::string>(_value) << ")";
    } else {
        throw std::runtime_error("Variant holds unsupported type '" + std::to_string(_value.index()) + "'");
    }

    return ss.str();
}

std::string Token::getParsingInformation() const
{
    if (std::holds_alternative<int>(_value)) {
        return std::to_string(std::get<int>(_value));
    } else if (std::holds_alternative<char>(_value)) {
        return std::string(1, std::get<char>(_value));
    } else if (std::holds_alternative<std::nullptr_t>(_value)) {
        return "empty token";
    } else if (std::holds_alternative<std::string>(_value)) {
        return std::get<std::string>(_value);
    } else {
        throw std::runtime_error("Variant holds unsupported type '" + std::to_string(_value.index()) + "'");
    }
}

std::variant<int, float, bool> Token::getFlexNumber() const
{
    if (std::holds_alternative<int>(_value)) {
        return std::get<int>(_value);
    } else if (std::holds_alternative<float>(_value)) {
        return std::get<float>(_value);
    } else if (std::holds_alternative<bool>(_value)) {
        return std::get<bool>(_value);
    } else {
        throw std::runtime_error("Variant does not hold integer, float or bool inside, cannot return value of it");
    }
}

std::string Token::getStringValue() const
{
    if (std::holds_alternative<std::string>(_value)) {
        return std::get<std::string>(_value);
    }

    throw std::runtime_error("Variant does not hold std::string inside, cannot return value of it");
}