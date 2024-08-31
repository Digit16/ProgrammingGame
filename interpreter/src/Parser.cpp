#include "Parser.h"

void Parser::eat(TokenType tokenType)
{
    if (_currentToken.getType() == tokenType) {
        _currentToken = _lexer.getNextToken();
    } else {
        raiseInvalidSyntaxError();
    }
}

/*
factor : PLUS factor
       | MINUS factor
       | INTEGER
       | FLOATING_NUMBER
       | LPAREN expr RPAREN
       | variable
*/
std::shared_ptr<AstNode> Parser::factor()
{
    Token token = _currentToken;

    if (token.getType() == TokenType::PLUS) {
        eat(TokenType::PLUS);

        return std::make_shared<UnaryOp>(token, factor());
    } else if (token.getType() == TokenType::MINUS) {
        eat(TokenType::MINUS);

        return std::make_shared<UnaryOp>(token, factor());
    } else if (token.getType() == TokenType::INTEGER) {
        eat(TokenType::INTEGER);

        return std::make_shared<Number>(token);
    } else if (token.getType() == TokenType::FLOATING_NUMBER) {
        eat(TokenType::FLOATING_NUMBER);

        return std::make_shared<Number>(token);
    } else if (token.getType() == TokenType::LPAREN) {
        eat(TokenType::LPAREN);
        std::shared_ptr<AstNode> result = expr();
        eat(TokenType::RPAREN);

        auto binOp = std::dynamic_pointer_cast<BinaryOperation>(result);
        if (!binOp) {
            throw std::runtime_error("Casting to BinaryOperation failed in LPAREN");
        }

        return std::make_shared<BinaryOperation>(*binOp);
    } else {
        return variable();
    }
}

// term : factor ((MUL | DIV) factor)*
std::shared_ptr<AstNode> Parser::term()
{
    std::shared_ptr<AstNode> node = factor();
    while ((_currentToken.getType() == TokenType::MULTIPLICATION) || (_currentToken.getType() == TokenType::DIVISION)) {
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
std::shared_ptr<AstNode> Parser::expr()
{
    std::shared_ptr<AstNode> node = term();
    while ((_currentToken.getType() == TokenType::PLUS) || (_currentToken.getType() == TokenType::MINUS)
           || (_currentToken.getType() == TokenType::COMPARISON) || (_currentToken.getType() == TokenType::GREATER)
           || (_currentToken.getType() == TokenType::LESS) || (_currentToken.getType() == TokenType::GREATER_EQUAL)
           || (_currentToken.getType() == TokenType::LESS_EQUAL)) {
        const Token token = _currentToken;

        if (token.getType() == TokenType::PLUS) {
            eat(TokenType::PLUS);
        } else if (token.getType() == TokenType::MINUS) {
            eat(TokenType::MINUS);
        } else if (token.getType() == TokenType::COMPARISON) {
            eat(TokenType::COMPARISON);
        } else if (token.getType() == TokenType::GREATER) {
            eat(TokenType::GREATER);
        } else if (token.getType() == TokenType::LESS) {
            eat(TokenType::LESS);
        } else if (token.getType() == TokenType::GREATER_EQUAL) {
            eat(TokenType::GREATER_EQUAL);
        } else if (token.getType() == TokenType::LESS_EQUAL) {
            eat(TokenType::LESS_EQUAL);
        }

        const auto left = node;
        const auto right = term();
        node = std::make_shared<BinaryOperation>(left, token, right);
    }

    return node;
}

/*
program : section DOT
*/
std::shared_ptr<AstNode> Parser::program()
{
    std::shared_ptr<AstNode> node = section();
    eat(TokenType::DOT);
    return node;
}

/*
section: START statementList END
*/
std::shared_ptr<AstNode> Parser::section()
{
    eat(TokenType::START);
    std::vector<std::shared_ptr<AstNode>> nodes = statementList();
    eat(TokenType::END);

    return std::make_shared<Section>(nodes);
}

/*
statement : section
          | assignmentStatement
          | ifStatement
          | functionDeclaration
          | functionCall
          | whileLoop
          | forLoop
          | empty
*/
std::shared_ptr<AstNode> Parser::statement()
{
    if (_currentToken.getType() == TokenType::START) {
        return section();
    } else if (_currentToken.getType() == TokenType::IF) {
        return ifStatement();
    } else if (_currentToken.getType() == TokenType::WHILE) {
        return whileStatement();
    } else if (_currentToken.getType() == TokenType::FOR) {
        return forStatement();
    } else if (_currentToken.getType() == TokenType::ID) {
        if (_lexer.peekNextToken().getType() == TokenType::LPAREN) {
            return functionCall();
        } else {
            return assignmentStatement();
        }
    } else if (_currentToken.getType() == TokenType::FUN) {
        return functionDeclaration();
    } else {
        return empty();
    }
}

/*
statementList : statement
               | statement SEMI statementList
*/
std::vector<std::shared_ptr<AstNode>> Parser::statementList()
{
    std::vector<std::shared_ptr<AstNode>> nodes;

    nodes.push_back(statement());

    while (_currentToken.getType() == TokenType::SEMICOLON) {
        eat(TokenType::SEMICOLON);
        nodes.push_back(statement());
    }

    if (_currentToken.getType() == TokenType::ID) {
        raiseInvalidSyntaxError();
    }

    return nodes;
}

/*
assignmentStatement : variable ASSIGN expr
*/
std::shared_ptr<AstNode> Parser::assignmentStatement()
{
    auto left = variable();
    auto token = _currentToken;
    eat(TokenType::ASSIGN);
    auto right = expr();

    return std::make_shared<Assign>(left, token, right);
}

/*
variable : ID
*/
std::shared_ptr<AstNode> Parser::variable()
{
    std::shared_ptr<AstNode> node = std::make_shared<Variable>(_currentToken);
    eat(TokenType::ID);

    return node;
}

/*
An empty production
*/
std::shared_ptr<AstNode> Parser::empty()
{
    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> Parser::ifStatement()
{
    eat(TokenType::IF);
    auto condition = expr();
    //SPDLOG_INFO("Condition NodeType = '{}'", getTypeString(condition->nodeType()));
    eat(TokenType::COLON);
    auto thenBranch = section();
    std::shared_ptr<AstNode> elseBranch = nullptr;

    if (_currentToken.getType() == TokenType::ELSE) {
        eat(TokenType::ELSE);
        eat(TokenType::COLON);
        elseBranch = section();
    }

    return std::make_shared<IfStatement>(condition, thenBranch, elseBranch);
}

/*
functionDeclaration : FUN ID LPAREN RPAREN COLON START statementList END SEMI
*/
std::shared_ptr<AstNode> Parser::functionDeclaration()
{
    eat(TokenType::FUN);
    std::string functionName = _currentToken.getStringValue();
    eat(TokenType::ID);
    eat(TokenType::LPAREN);
    eat(TokenType::RPAREN);
    eat(TokenType::COLON);
    eat(TokenType::START);

    std::vector<std::shared_ptr<AstNode>> body = statementList();

    eat(TokenType::END);

    return std::make_shared<FunDeclaration>(functionName, body);
}

/*
functionCall : ID LPAREN RPAREN
*/
std::shared_ptr<AstNode> Parser::functionCall()
{
    std::string functionName = _currentToken.getStringValue();
    eat(TokenType::ID);
    eat(TokenType::LPAREN);
    eat(TokenType::RPAREN);

    return std::make_shared<FunCall>(functionName);
}

std::shared_ptr<AstNode> Parser::whileStatement()
{
    eat(TokenType::WHILE);
    auto condition = expr();
    eat(TokenType::START);
    auto body = section();

    return std::make_shared<WhileLoop>(condition, body);
}

std::shared_ptr<AstNode> Parser::forStatement()
{
    eat(TokenType::FOR);
    eat(TokenType::LPAREN);
    auto initialization = assignmentStatement();
    eat(TokenType::SEMICOLON);
    auto condition = expr();
    eat(TokenType::SEMICOLON);
    auto increment = assignmentStatement();
    eat(TokenType::RPAREN);
    auto body = section();

    return std::make_shared<ForLoop>(initialization, condition, increment, body);
}

std::shared_ptr<AstNode> Parser::parse()
{
    auto node = program();
    if (_currentToken.getType() != TokenType::END_OF_FILE) {
        raiseInvalidSyntaxError();
    }

    return node;
}