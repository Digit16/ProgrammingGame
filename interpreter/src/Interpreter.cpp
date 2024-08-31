#include "Interpreter.h"
#include "GlobalScope.h"


using Visitor = NodeVisitor::VisitNode;


std::map<std::string, std::variant<int, float, bool>> GLOBAL_SCOPE;
std::unordered_map<std::string, std::vector<std::shared_ptr<AstNode>>> GLOBAL_FUNCTIONS;


std::shared_ptr<AstNode> Visitor::operator()(BinaryOperation& node)
{

    NodeVariant left = getVariant(node.left());
    NodeVariant right = getVariant(node.right());

    TokenType tokenType = node.binaryOperator().getType();
    auto arithmeticIt = std::find(Token::arithmeticTokenTypes.begin(), Token::arithmeticTokenTypes.end(), tokenType);
    if (arithmeticIt != Token::arithmeticTokenTypes.end()) {
        FlexNumber value = calculateBinaryResult(visit(left), visit(right), tokenType);
        if (std::holds_alternative<int>(value)) {
            return std::make_shared<Number>(Token(std::get<int>(value), TokenType::INTEGER));
        } else if (std::holds_alternative<float>(value)) {
            return std::make_shared<Number>(Token(std::get<float>(value), TokenType::FLOATING_NUMBER));
        } else {
            throw std::runtime_error("Boolean value not supported as result of calculateBinaryResult in arithmetic operation");
        }
    }

    auto comparisonIt = std::find(Token::comparisonTokenTypes.begin(), Token::comparisonTokenTypes.end(), tokenType);
    if (comparisonIt != Token::comparisonTokenTypes.end()) {
        FlexNumber value = calculateBinaryResult(visit(left), visit(right), tokenType);
        if (std::holds_alternative<int>(value)) {
            throw std::runtime_error("Int value not supported as result of calculateBinaryResult in comparison");
        } else if (std::holds_alternative<float>(value)) {
            throw std::runtime_error("Float value not supported as result of calculateBinaryResult in comparison");
        } else {
            return std::make_shared<Number>(Token(std::get<bool>(value), TokenType::BOOL_VALUE));
        }
    }

    throw std::runtime_error("Unknown type of binary operator in visitor for BinaryOperation '" + Token::typeToString(tokenType) + "'");
}

std::shared_ptr<AstNode> Visitor::operator()(Number& node)
{
    auto value = node.value();
    if (std::holds_alternative<int>(value)) {
        return std::make_shared<Number>(Token(std::get<int>(value), TokenType::INTEGER));
    } else if (std::holds_alternative<float>(value)) {
        return std::make_shared<Number>(Token(std::get<float>(value), TokenType::FLOATING_NUMBER));
    } else if (std::holds_alternative<bool>(value)) {
        return std::make_shared<Number>(Token(std::get<bool>(value), TokenType::BOOL_VALUE));
    } else {
        throw std::runtime_error("Unknown variant value in Number visitor");
    }
}

std::shared_ptr<AstNode> Visitor::operator()(UnaryOp& node)
{
    NodeVariant exprVariant = getVariant(node.expr());
    auto res = visit(exprVariant);

    auto number = std::dynamic_pointer_cast<Number>(res);
    if (!number) {
        throw std::runtime_error("Casting failed in UnaryOp visit");
    }

    auto value = number->value();

    TokenType type = node.token().getType();
    if (std::holds_alternative<int>(value)) {
        int intValue = std::get<int>(value);
        if (type == TokenType::PLUS) {
            return std::make_shared<Number>(Token(intValue, TokenType::INTEGER));
        } else if (type == TokenType::MINUS) {
            return std::make_shared<Number>(Token(-intValue, TokenType::INTEGER));
        } else {
            throw std::runtime_error("Unknown operator type for int value in UnaryOp visit");
        }
    } else {
        float floatValue = std::get<float>(value);
        if (type == TokenType::PLUS) {
            return std::make_shared<Number>(Token(floatValue, TokenType::FLOATING_NUMBER));
        } else if (type == TokenType::MINUS) {
            return std::make_shared<Number>(Token(-floatValue, TokenType::FLOATING_NUMBER));
        } else {
            throw std::runtime_error("Unknown operator type for float value in UnaryOp visit");
        }
    }
}

std::shared_ptr<AstNode> Visitor::operator()(Section& node)
{
    for (const auto& child : node.children()) {
        NodeVisitor::visit(getVariant(child));
    }

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> Visitor::operator()(Assign& node)
{
    auto variable = std::dynamic_pointer_cast<Variable>(node.left());
    if (!variable) {
        throw std::runtime_error("Casting to Variable in VisitAssign failed!");
    }

    auto value = std::dynamic_pointer_cast<Number>(visit(getVariant(node.right())));
    if (!value) {
        throw std::runtime_error("Casting to Number in VisitAssign failed!");
    }
    GLOBAL_SCOPE[variable->name()] = value->value();

    return std::make_shared<EmptyNode>();
}
std::shared_ptr<AstNode> Visitor::operator()(Variable& node)
{
    const std::string& variableName = node.name();
    std::variant<int, float, bool> value = GLOBAL_SCOPE.at(variableName);

    if (std::holds_alternative<int>(value)) {
        return std::make_shared<Number>(Token(std::get<int>(value), TokenType::INTEGER));
    } else if (std::holds_alternative<float>(value)) {
        return std::make_shared<Number>(Token(std::get<float>(value), TokenType::FLOATING_NUMBER));
    } else if (std::holds_alternative<bool>(value)) {
        return std::make_shared<Number>(Token(std::get<bool>(value), TokenType::BOOL_VALUE));
    } else {
        throw std::runtime_error("In Visit::Variable, variant does not hold int, float or bool");
    }
}

std::shared_ptr<AstNode> Visitor::operator()(EmptyNode&)
{
    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> Visitor::operator()(IfStatement& node)
{
    auto conditionResult = visit(getVariant(node.condition()));
    auto conditionValue = std::dynamic_pointer_cast<Number>(conditionResult);

    if (!conditionValue) {
        throw std::runtime_error("Condition expression in if-statement did not return a Number.");
    }

    FlexNumber condition = conditionValue->value();
    bool conditionIsTrue = (std::holds_alternative<bool>(condition) && std::get<bool>(condition)) // It is usual flow for comparisons
                           || (std::holds_alternative<int>(condition) && std::get<int>(condition) != 0)
                           || (std::holds_alternative<float>(condition) && std::get<float>(condition) != 0.0f);

    if (conditionIsTrue) {
        return visit(getVariant(node.thenBranch()));
    } else if (node.elseBranch()) {
        return visit(getVariant(node.elseBranch()));
    } else {
        return std::make_shared<EmptyNode>();
    }
}

std::shared_ptr<AstNode> Visitor::operator()(FunDeclaration& node)
{
    GLOBAL_FUNCTIONS[node.name()] = node.body();

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> Visitor::operator()(FunCall& node)
{
    auto it = GLOBAL_FUNCTIONS.find(node.name());
    if (it == GLOBAL_FUNCTIONS.end()) {
        throw std::runtime_error("Function '" + node.name() + "' is not defined.");
    }

    const auto& functionBody = it->second;
    for (const auto& statement : functionBody) {
        visit(getVariant(statement));
    }

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> Visitor::operator()(WhileLoop& node)
{
    while (true) {
        auto conditionResult = visit(getVariant(node.condition()));
        auto conditionValue = std::dynamic_pointer_cast<Number>(conditionResult);

        if (!conditionValue) {
            throw std::runtime_error("Condition expression in while-loop did not return a Number.");
        }

        bool conditionIsTrue = (std::holds_alternative<int>(conditionValue->value()) && std::get<int>(conditionValue->value()) != 0)
                               || (std::holds_alternative<float>(conditionValue->value()) && std::get<float>(conditionValue->value()) != 0.0f);

        if (!conditionIsTrue) {
            break;
        }

        visit(getVariant(node.body()));
    }

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> Visitor::operator()(ForLoop& node)
{
    visit(getVariant(node.initialization()));

    while (true) {
        auto conditionResult = visit(getVariant(node.condition()));
        auto conditionValue = std::dynamic_pointer_cast<Number>(conditionResult);

        if (!conditionValue) {
            throw std::runtime_error("Condition expression in for-loop did not return a Number.");
        }

        bool conditionIsTrue = (std::holds_alternative<int>(conditionValue->value()) && std::get<int>(conditionValue->value()) != 0)
                               || (std::holds_alternative<float>(conditionValue->value()) && std::get<float>(conditionValue->value()) != 0.0f);

        if (!conditionIsTrue) {
            break;
        }

        visit(getVariant(node.body()));

        visit(getVariant(node.increment()));
    }

    return std::make_shared<EmptyNode>();
}

// static
std::shared_ptr<AstNode> NodeVisitor::visit(NodeVariant astNode)
{
    return std::visit(VisitNode(), astNode);
}

std::shared_ptr<AstNode> Interpreter::interpret(const std::string& text)
{
    Lexer lexer(text);
    Parser parser(lexer);

    std::shared_ptr<AstNode> tree = parser.parse();

    return NodeVisitor::visit(getVariant(tree));
}

// static
FlexNumber NodeVisitor::calculateBinaryResult(const std::shared_ptr<AstNode>& left, const std::shared_ptr<AstNode>& right, TokenType type)
{
    // TODO(wkubski): Currently we only support comparisons, where variable is on the left side of operator
    if (left->nodeType() == NodeType::NUMBER) {
        auto leftNumber = std::dynamic_pointer_cast<Number>(left);
        auto rightNumber = std::dynamic_pointer_cast<Number>(right);

        if ((!leftNumber) || (!rightNumber)) {
            throw std::runtime_error("Casting failed in calculateBinaryResult with leftNode == Number");
        }

        FlexNumber leftValue = leftNumber->value();
        FlexNumber rightValue = rightNumber->value();

        if (type == TokenType::PLUS) {
            return leftValue + rightValue;
        } else if (type == TokenType::MINUS) {
            return leftValue - rightValue;
        } else if (type == TokenType::MULTIPLICATION) {
            return leftValue * rightValue;
        } else if (type == TokenType::DIVISION) {
            return leftValue / rightValue;
        } else if (type == TokenType::COMPARISON) {
            return leftValue == rightValue;
        } else if (type == TokenType::NOT_EQUAL) {
            return leftValue != rightValue;
        } else if (type == TokenType::GREATER) {
            return leftValue > rightValue;
        } else if (type == TokenType::LESS) {
            return leftValue < rightValue;
        } else if (type == TokenType::GREATER_EQUAL) {
            return leftValue >= rightValue;
        } else if (type == TokenType::LESS_EQUAL) {
            return leftValue <= rightValue;
        } else {
            throw std::runtime_error("Unknown token type in calculateBinaryResult");
        }
    } else if (left->nodeType() == NodeType::VARIABLE) {
        auto leftVariable = std::dynamic_pointer_cast<Variable>(left);
        auto rightNumber = std::dynamic_pointer_cast<Number>(right);

        if ((!leftVariable) || (!rightNumber)) {
            throw std::runtime_error("Casting failed in calculateBinaryResult with leftNode == Variable");
        }

        FlexNumber leftValue = GLOBAL_SCOPE[leftVariable->name()];
        FlexNumber rightValue = rightNumber->value();

        if (type == TokenType::PLUS) {
            return leftValue + rightValue;
        } else if (type == TokenType::MINUS) {
            return leftValue - rightValue;
        } else if (type == TokenType::MULTIPLICATION) {
            return leftValue * rightValue;
        } else if (type == TokenType::DIVISION) {
            return leftValue / rightValue;
        } else if (type == TokenType::COMPARISON) { // This and all below should be only choice here, according to logic
            return leftValue == rightValue;
        } else if (type == TokenType::NOT_EQUAL) {
            return leftValue != rightValue;
        } else if (type == TokenType::GREATER) {
            return leftValue > rightValue;
        } else if (type == TokenType::LESS) {
            return leftValue < rightValue;
        } else if (type == TokenType::GREATER_EQUAL) {
            return leftValue >= rightValue;
        } else if (type == TokenType::LESS_EQUAL) {
            return leftValue <= rightValue;
        } else {
            throw std::runtime_error("Unknown token type in calculateBinaryResult");
        }

    } else {
        throw std::runtime_error("Left node of BinaryOperator is neither Number nor Variable, execution not possible");
    }
}
