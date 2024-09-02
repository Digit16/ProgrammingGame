#pragma once

#include "Ast.h"
#include "Lexer.h"

#include <algorithm>
#include <vector>

class Parser
{
public:
    explicit Parser() :
        _lexer(Lexer()),
        _currentToken(Token(std::nullptr_t(), TokenType::END_OF_FILE))
    {
    }

    explicit Parser(const Lexer& lexer) :
        _lexer(lexer),
        _currentToken(_lexer.getNextToken())
    {
    }

    void raiseParsingError(const std::vector<TokenType>& expectedTokenType);

    void eat(TokenType tokenType);

    std::shared_ptr<AstNode> factor();

    std::shared_ptr<AstNode> term();

    std::shared_ptr<AstNode> expr();

    std::shared_ptr<AstNode> program();

    std::shared_ptr<AstNode> section();

    std::shared_ptr<AstNode> statement();

    std::vector<std::shared_ptr<AstNode>> statementList();

    std::shared_ptr<AstNode> assignmentStatement();

    std::shared_ptr<AstNode> variable();

    std::shared_ptr<AstNode> empty();

    std::shared_ptr<AstNode> parse();

    std::shared_ptr<AstNode> ifStatement();

    std::shared_ptr<AstNode> functionDeclaration();

    std::shared_ptr<AstNode> functionCall();

    std::shared_ptr<AstNode> whileStatement();

    std::shared_ptr<AstNode> forStatement();

    Lexer lexer() { return _lexer; }

private:
    // class ParsingException : public std::exception
    // {
    // private:
    //     const char* message;

    // public:
    //     ParsingException(const char* msg) :
    //         message(msg)
    //     {
    //     }
    //     char* what() { return message; }
    // };

    Lexer _lexer;
    Token _currentToken;
};
