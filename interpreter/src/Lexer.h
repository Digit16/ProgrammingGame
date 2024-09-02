#pragma once

#include "Token.h"

#include <map>

class Lexer
{
public:
    constexpr static size_t VAR_DECLARATION_LENGTH = 5;
    constexpr static size_t IF_DECLARATION_LENGTH = 3;

    Lexer() :
        _text(std::string()),
        _pos(-1),
        _currentChar('\0'),
        _parsingLine(-1),
        _parsingPosition(-1)
    {
    }

    Lexer(const std::string& text) :
        _text(text),
        _pos(0),
        _currentChar(_text.at(_pos)),
        _parsingLine(0),
        _parsingPosition(0)
    {
    }

    void raiseInvalidCharacterError() const { throw std::runtime_error("Invalid character"); }

    void advance();

    void skipWhitespace();

    void skipNewLine();

    std::variant<int, float, bool> number();

    Token getNextToken();
    Token peekNextToken();

    char peekNextChar();

    Token id();

    int16_t getParsingLine() { return _parsingLine; }
    int16_t getParsingPosition() { return _parsingPosition; }

private:
    std::string _text;
    int _pos;
    char _currentChar;

    int16_t _parsingLine;
    int16_t _parsingPosition;
};
