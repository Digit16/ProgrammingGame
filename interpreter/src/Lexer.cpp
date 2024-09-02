#include "Lexer.h"

#include <iostream>

void Lexer::advance()
{
    ++_pos;
    if (_pos > static_cast<int>(_text.length()) - 1) {
        _currentChar = '\0';
    } else {
        _currentChar = _text[_pos];
    }
    ++_parsingPosition;
}

void Lexer::skipWhitespace()
{
    while (_currentChar != '\0' && _currentChar == ' ') {
        advance();
    }
}

void Lexer::skipNewLine()
{
    ++_pos;
    if (_pos > static_cast<int>(_text.length()) - 1) {
        _currentChar = '\0';
    } else {
        _currentChar = _text[_pos];
    }

    ++_parsingLine;
    _parsingPosition = 0;
}

std::variant<int, float, bool> Lexer::number()
{
    std::string result;
    while (_currentChar != '\0' && isdigit(_currentChar)) {
        result += _currentChar;
        advance();
    }

    if (_currentChar == '.') {
        result += _currentChar;
        advance();

        while (_currentChar != '\0' && isdigit(_currentChar)) {
            result += _currentChar;
            advance();
        }

        return std::stof(result);
    } else {
        return std::stoi(result);
    }
}

Token Lexer::getNextToken()
{
    while (_currentChar != '\0') {
        if (_currentChar == ' ') {
            skipWhitespace();
            continue;
        } else if (_currentChar == '\n') {
            skipNewLine();
            continue;
        } else if (_currentChar == '+') {
            advance();
            return Token('+', TokenType::PLUS);
        } else if (_currentChar == '-') {
            advance();
            return Token('-', TokenType::MINUS);
        } else if (_currentChar == '*') {
            advance();
            return Token('*', TokenType::MULTIPLICATION);
        } else if (_currentChar == '/') {
            advance();
            return Token('/', TokenType::DIVISION);
        } else if (_currentChar == '=' && peekNextChar() == '=') {
            advance();
            advance();
            return Token("==", TokenType::COMPARISON);
        } else if (_currentChar == '!' && peekNextChar() == '=') {
            advance();
            advance();
            return Token("!=", TokenType::NOT_EQUAL);
        } else if (_currentChar == '>' && peekNextChar() == '=') {
            advance();
            advance();
            return Token(">=", TokenType::GREATER_EQUAL);
        } else if (_currentChar == '<' && peekNextChar() == '=') {
            advance();
            advance();
            return Token("<=", TokenType::LESS_EQUAL);
        } else if (_currentChar == '>') {
            advance();
            advance();
            return Token(">", TokenType::GREATER);
        } else if (_currentChar == '<') {
            advance();
            advance();
            return Token("<", TokenType::LESS);
        } else if (_currentChar == '(') {
            advance();
            return Token('(', TokenType::LPAREN);
        } else if (_currentChar == ')') {
            advance();
            return Token(')', TokenType::RPAREN);
        } else if (_currentChar == '=') {
            advance();
            return Token('=', TokenType::ASSIGN);
        } else if (_currentChar == ':') {
            advance();
            return Token(':', TokenType::COLON);
        } else if (_currentChar == ';') {
            advance();
            return Token(';', TokenType::SEMICOLON);
        } else if (_currentChar == '.') {
            advance();
            return Token('.', TokenType::DOT);
        } else if (_currentChar == ',') {
            advance();
            return Token(',', TokenType::COMMA);
        } else if (isdigit(_currentChar)) {
            std::variant<int, float, bool> num = number();
            if (std::holds_alternative<int>(num)) {
                return Token(std::get<int>(num), TokenType::INTEGER);
            } else {
                return Token(std::get<float>(num), TokenType::FLOATING_NUMBER);
            }
        } else if (isalpha(_currentChar)) {
            return id();
        } else {

            raiseInvalidCharacterError();
        }
    }

    return Token(std::nullptr_t(), TokenType::END_OF_FILE);
}

Token Lexer::peekNextToken()
{
    return Lexer(*this).getNextToken();
}

char Lexer::peekNextChar()
{
    return this->_text[this->_pos + 1];
}

Token Lexer::id()
{
    std::string result;
    std::map<std::string, Token> RESERVED_KEYWORDS{
        {"START", Token("START", TokenType::START)},
        {"END",   Token("END",   TokenType::END)  },
        {"fun",   Token("fun",   TokenType::FUN)  },
        {"if",    Token("if",    TokenType::IF)   },
        {"else",  Token("else",  TokenType::ELSE) },
        {"for",   Token("for",   TokenType::FOR)  },
        {"while", Token("while", TokenType::WHILE)}
    };

    while (this->_currentChar != '\0' && isalnum(this->_currentChar)) {
        result += this->_currentChar;
        this->advance();
    }

    if (result == "auto") {
        advance();
        std::string varResult;
        while (this->_currentChar != '\0' && isalnum(this->_currentChar)) {
            varResult += this->_currentChar;
            this->advance();
        }

        if (RESERVED_KEYWORDS.count(varResult)) {
            throw std::runtime_error(varResult + " is reserved keyword!");
        } else {
            return Token(varResult, TokenType::ID);
        }
    } else {
        if (RESERVED_KEYWORDS.count(result)) {
            return RESERVED_KEYWORDS.at(result);
        } else {
            return Token(result, TokenType::ID);
        }
    }
}
