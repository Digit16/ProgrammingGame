#include "Interpreter.h"

// static
std::string Token::typeToString(TokenType type) {
    switch(type) {
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

    throw std::runtime_error("TokenType not implemented, string conversion is not possible");
}

std::string Token::toString() const {
    if(std::holds_alternative<int>(_value)) {
        return std::string("TOKEN(" + typeToString(_type) + "," + std::to_string(std::get<int>(_value)) +  ")");
    } else if(std::holds_alternative<char>(_value)) {
        return std::string("TOKEN(" + typeToString(_type) + "," + std::get<char>(_value) +  ")");
    } else if(std::holds_alternative<std::nullptr_t>(_value)) {
        return std::string("TOKEN(" + typeToString(_type) + ",NONE)");
    } else {
        throw std::runtime_error(std::string("Variant holds unsupported type '" + std::to_string(_value.index()) + "'"));
    }
}

int Token::getIntegerValue() const {
    if(std::holds_alternative<int>(_value)) {
        return std::get<int>(_value);
    }

    throw std::runtime_error("Variant does not hold integer inside, cannot return value of it");
}

std::variant<Number, BinaryOperation> NodeConverter::getVariant(const std::shared_ptr<AstNode>& node) {
    switch (node->nodeType())
    {
    case NodeType::NUMBER: {
        auto number = std::dynamic_pointer_cast<Number>(node);
        if (!number) {
            throw std::runtime_error("Casting failed");
        }
        
        return std::variant<Number, BinaryOperation>(Number(*number));
    }
    case NodeType::BINARY_OPERATION: {
        auto binOp = std::dynamic_pointer_cast<BinaryOperation>(node);
        if (!binOp) {
            throw std::runtime_error("Casting failed");
        }
        
        return std::variant<Number, BinaryOperation>(BinaryOperation(*binOp));
    }
    }

    throw std::runtime_error("Unknown NodeType");
}

void Lexer::advance() {
    ++_pos;
    if (_pos > static_cast<int>(_text.length()) - 1) {
        _currentChar = '\0';
    } else {
        _currentChar = _text[_pos];
    }
}

void Lexer::skipWhitespace() {
    while (_currentChar != '\0' && _currentChar == ' ') {
        advance();
    }
}

int Lexer::integer() {
    std::string result;
    while (_currentChar != '\0' && isdigit(_currentChar)) {
        result += _currentChar;
        advance();
    }
            
    return std::stoi(result);
}

Token Lexer::getNextToken() {
    while (_currentChar != '\0')
    {
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

void Parser::eat(TokenType tokenType) {
    if (_currentToken.getType() == tokenType) {
        _currentToken = _lexer.getNextToken();
    } else {
        raiseInvalidSyntaxError();
    }
}

// factor : INTEGER | LPAREN expr RPAREN
std::shared_ptr<AstNode> Parser::factor() {
    Token token = _currentToken;
    if (token.getType() == TokenType::INTEGER) {
        eat(TokenType::INTEGER);

        return std::make_shared<Number>(token);
    } else if (token.getType() == TokenType::LPAREN) {
        eat(TokenType::LPAREN);
        std::shared_ptr<AstNode> result = expr();
        eat(TokenType::RPAREN);

        auto binOp = std::dynamic_pointer_cast<BinaryOperation>(result);
        if (!binOp) {
            throw std::runtime_error("Casting failed");
        }

        return std::make_shared<BinaryOperation>(*binOp);
    } else {
        throw std::runtime_error("Unknown token type in factor method");
    }
}

// term : factor ((MUL | DIV) factor)*
std::shared_ptr<AstNode> Parser::term() {
    std::shared_ptr<AstNode> node = factor();
    while ((_currentToken.getType() == TokenType::MULTIPLICATION) ||
           (_currentToken.getType() == TokenType::DIVISION)) {
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
std::shared_ptr<AstNode> Parser::expr() {
    std::shared_ptr<AstNode> node = term();
    while ((_currentToken.getType() == TokenType::PLUS) ||
           (_currentToken.getType() == TokenType::MINUS)) {
        const Token token = _currentToken;

        if (token.getType() == TokenType::PLUS) {
            eat(TokenType::PLUS);
        } else if (token.getType() == TokenType::MINUS) {
            eat(TokenType::MINUS);
        }

        const auto left = node;
        const auto right = term();
        node = std::make_shared<BinaryOperation>(left, token, right);
    }

    return node;
}

using Visitor = NodeVisitor::VisitNode;

// std::shared_ptr<AstNode> Visitor::operator()(auto& node) { throw std::runtime_error("Unsupported AstNode in NodeVisitor"); }

std::shared_ptr<AstNode> Visitor::operator()(BinaryOperation& node) { 

    std::variant<Number, BinaryOperation> left = NodeConverter::getVariant(node.left());
    std::variant<Number, BinaryOperation> right = NodeConverter::getVariant(node.right());

    TokenType tokenType = node.binaryOperator().getType();
    if (tokenType == TokenType::PLUS) {
        int value = calculateResult(visit(left), visit(right), tokenType);
        return std::make_shared<Number>(Token(value, TokenType::INTEGER));
    } else if (tokenType == TokenType::MINUS) {
        int value = calculateResult(visit(left), visit(right), tokenType);
        return std::make_shared<Number>(Token(value, TokenType::INTEGER));
    } else if (tokenType == TokenType::MULTIPLICATION) {
        int value = calculateResult(visit(left), visit(right), tokenType);
        return std::make_shared<Number>(Token(value, TokenType::INTEGER));
    } else if (tokenType == TokenType::DIVISION) {
        int value = calculateResult(visit(left), visit(right), tokenType);
        return std::make_shared<Number>(Token(value, TokenType::INTEGER));
    } else {
        throw std::runtime_error("Unknown type of binary operator in visitor");
    }
}

std::shared_ptr<AstNode> Visitor::operator()(Number& node) {
    return std::make_shared<Number>(Token(node.value(), TokenType::INTEGER));
}

std::shared_ptr<AstNode> Interpreter::interpret(const std::string& text) {
    Lexer lexer{text};
    Parser parser{lexer};

    std::shared_ptr<AstNode> tree = parser.parse();
    return visit(NodeConverter::getVariant(tree));
}

// static
std::shared_ptr<AstNode> NodeVisitor::visit(std::variant<Number, BinaryOperation> astNode) {
    return std::visit(VisitNode(), astNode);
}

// static
int NodeVisitor::calculateResult(const std::shared_ptr<AstNode>& left, const std::shared_ptr<AstNode>& right, TokenType type)
{
    auto leftNumber = std::dynamic_pointer_cast<Number>(left);
    auto rightNumber = std::dynamic_pointer_cast<Number>(right);

    if ((!leftNumber) || (!rightNumber)) {
        throw std::runtime_error("Casting failed");
    }

    int leftValue = leftNumber->value();
    int rightValue = rightNumber->value();

    if(type == TokenType::PLUS) {
        return leftValue + rightValue;
    } else if(type == TokenType::MINUS) {
        return leftValue - rightValue;
    } else if(type == TokenType::MULTIPLICATION) {
        return leftValue * rightValue;
    } else if(type == TokenType::DIVISION){
        return leftValue / rightValue;
    } else {
        throw std::runtime_error("Unknown token type in calculateResult");
    }
}
