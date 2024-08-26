#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <variant>

enum class TokenType : uint8_t
{
    INTEGER = 1,
    FLOATING_NUMBER = 2,
    PLUS = 3,
    MINUS = 4,
    MULTIPLICATION = 5,
    DIVISION = 6,
    LPAREN = 7,
    RPAREN = 8,
    END_OF_FILE = 9,
    NONE = 10,
    ID = 11,
    START = 12,
    END = 13,
    ASSIGN = 14,
    COLON = 15,
    SEMICOLON = 16,
    DOT = 17,
    COMMA = 18,
    FUN = 20,
    IF = 21,
    ELSE = 22,
    FOR = 23,
    WHILE = 24,
    COMPARISON = 25,
    BOOL_VALUE = 26,
    NOT_EQUAL = 27,
    GREATER = 28,
    LESS = 29,
    GREATER_EQUAL = 30,
    LESS_EQUAL = 31
};

class Token
{
public:
    static constexpr std::array<TokenType, 4> arithmeticTokenTypes{TokenType::PLUS, TokenType::MINUS, TokenType::MULTIPLICATION, TokenType::DIVISION};
    static constexpr std::array<TokenType, 6> comparisonTokenTypes{
        TokenType::COMPARISON, TokenType::NOT_EQUAL, TokenType::GREATER, TokenType::LESS, TokenType::GREATER_EQUAL, TokenType::LESS_EQUAL};

    Token(std::variant<int, float, char, std::nullptr_t, std::string, bool> value, TokenType type) :
        _value(value),
        _type(type){};

    static std::string typeToString(TokenType type);

    std::string toDebugString() const;

    std::variant<int, float, bool> getFlexNumber() const;
    std::string getStringValue() const;

    TokenType getType() const { return _type; }

private:
    std::variant<int, float, char, std::nullptr_t, std::string, bool> _value;
    TokenType _type;
};