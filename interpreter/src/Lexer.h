#pragma once

#include <map>

#include "Token.h"


class Lexer
{
public:
    constexpr static size_t VAR_DECLARATION_LENGTH = 5;
    constexpr static size_t IF_DECLARATION_LENGTH = 3;

    Lexer(const std::string& text) :
        _text(text),
        _pos(0),
        _currentChar(_text.at(_pos))
    {
    }

    void raiseInvalidCharacterError() const { throw std::runtime_error("Invalid character"); }

    void advance();

    void skipWhitespace();

    std::variant<int, float, bool> number();

    Token getNextToken();
    Token peekNextToken();

    char peekNextChar();

    Token id();

private:
    std::string _text;
    int _pos;
    char _currentChar;
};
