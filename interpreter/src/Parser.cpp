#include "Parser.h"

#include <iostream>

void Parser::registerBuiltInMethod(const std::string& functionName)
{
    _lexer.registerBuiltInMethod(functionName);
}

void Parser::raiseParsingError(const std::vector<TokenType>& expectedTokenType)
{
    if (expectedTokenType.empty()) {
        throw std::runtime_error("Cannot expect nothing!"); // Should never happen
    }

    std::string tokenTypeString = "(";

    for (auto type : expectedTokenType) {
        tokenTypeString += Token::typeToString(type) + "|";
    }
    tokenTypeString.pop_back();
    tokenTypeString += ")";

    const std::string message = "Parsing error at line " + std::to_string(lexer().getParsingLine()) + ", position "
                                + std::to_string(lexer().getParsingPosition()) + ", expected " + tokenTypeString + ", got "
                                + _currentToken.getParsingInformation();

    throw std::runtime_error(message);
}

void Parser::eat(TokenType tokenType)
{
    if (_currentToken.getType() == tokenType) {
        _currentToken = _lexer.getNextToken();
    } else {
        raiseParsingError({tokenType});
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
    } else if (token.getType() == TokenType::ID) {
        return variable();
    } else {
        raiseParsingError({TokenType::PLUS, TokenType::MINUS, TokenType::INTEGER, TokenType::FLOATING_NUMBER, TokenType::LPAREN, TokenType::ID});
    }

    return std::make_shared<EmptyNode>();
}
/*
term : factor ((MUL | DIV) factor)*
*/
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
          | variableDeclaration
          | functionDeclaration
          | builtInFunction
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
    } else if (_currentToken.getType() == TokenType::VARIABLE_DECLARATION) {
        return variableDeclaration();
    } else if (_currentToken.getType() == TokenType::ID) {
        if (std::string value = _currentToken.getStringValue(); value.at(value.size() - 2) == '(' && value.at(value.size() - 1) == ')') {
            return functionCall();
        } else {
            return assignmentStatement();
        }
    } else if (_currentToken.getType() == TokenType::FUN_DECLARATION) {
        return functionDeclaration();
    } else if (_currentToken.getType() == TokenType::BUILT_IN_FUNCTION) {
        return builtInFunction();
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
        throw std::runtime_error("Variable after semicolon in statement list");
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
variableDeclaration : auto ID | auto assignmentStatement
*/
std::shared_ptr<AstNode> Parser::variableDeclaration()
{
    eat(TokenType::VARIABLE_DECLARATION);
    if (_currentToken.getType() == TokenType::ID) {
        auto assign = assignmentStatement();
        return std::make_shared<VariableDeclaration>(assign, true);
    } else {
        auto variableNode = variable();
        return std::make_shared<VariableDeclaration>(variableNode);
    }
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

/*
ifStatement : IF EXPR COLON SECTION (ELSE COLON)
*/
std::shared_ptr<AstNode> Parser::ifStatement()
{
    eat(TokenType::IF);
    auto condition = expr();
    eat(TokenType::COLON);
    auto thenBranch = section();
    std::shared_ptr<AstNode> elseBranch = std::make_shared<EmptyNode>();

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
    eat(TokenType::FUN_DECLARATION);
    std::string functionName = _currentToken.getStringValue();
    eat(TokenType::ID);
    // eat(TokenType::LPAREN);
    // eat(TokenType::RPAREN);
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
    const std::string functionName = _currentToken.getStringValue();
    eat(TokenType::ID);
    // eat(TokenType::LPAREN);
    // eat(TokenType::RPAREN);

    return std::make_shared<FunCall>(functionName);
}

/*
whileStatement : WHILE EXPR START SECTION
*/
std::shared_ptr<AstNode> Parser::whileStatement()
{
    eat(TokenType::WHILE);
    auto condition = expr();
    eat(TokenType::START);
    auto body = section();

    return std::make_shared<WhileLoop>(condition, body);
}

/*
builtInFunction : builtInFunction
*/
std::shared_ptr<AstNode> Parser::builtInFunction()
{
    auto ct = _currentToken;
    eat(TokenType::BUILT_IN_FUNCTION);

    return std::make_shared<BuiltInFunction>(ct.getStringValue());
}

/*
forStatement : FOR LPAREN variableDeclaration SEMICOLON expr SEMICOLON assignmentStatement RPAREN section
*/
std::shared_ptr<AstNode> Parser::forStatement()
{
    eat(TokenType::FOR);
    eat(TokenType::LPAREN);
    auto initialization = variableDeclaration();
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
        raiseParsingError({TokenType::END_OF_FILE});
    }

    return node;
}