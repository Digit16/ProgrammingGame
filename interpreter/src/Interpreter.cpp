#include "Interpreter.h"

#include <iostream>

// using NodeVisitor = InterpreterNodeVisitor::VisitNode;
using NodeVisitor = Interpreter::VisitNode;
using stNodeVisitor = SymbolTableVisitNode;

void SymbolTableBuilder::build(std::shared_ptr<AstNode> tree, st::SymbolTable& st)
{
    SymbolTableVisitNode stvn(st);
    stvn.visit(getVariant(tree));
}

void st::SymbolTable::create(const std::shared_ptr<Symbol>& symbol)
{
    _symbols[symbol->name()] = symbol;
}

std::shared_ptr<st::Symbol> st::SymbolTable::find(const std::string& symbolName)
{
    if (auto search = _symbols.find(symbolName); search != _symbols.end()) {
        return search->second;
    } else {
        return nullptr;
    }
}

std::shared_ptr<st::Symbol> st::SymbolTable::findWithType(const std::string& symbolName, st::SymbolType st)
{
    if (auto search = _symbols.find(symbolName); search != _symbols.end()) {
        if (search->second->symbolType() != st) {
            const std::string errorMessage = "Symbol with name '" + symbolName + "' - type mismatch";
            throw std::runtime_error(errorMessage);
        }
        return search->second;
    } else {
        return nullptr;
    }
}

void st::SymbolTable::init()
{
    create(std::make_shared<BuiltInTypeSymbol>("INTEGER", SymbolVariableType::INTEGER));
    create(std::make_shared<BuiltInTypeSymbol>("FLOATING_NUMBER", SymbolVariableType::FLOATING_NUMBER));
    create(std::make_shared<BuiltInTypeSymbol>("BOOL_VALUE", SymbolVariableType::BOOL_VALUE));
}

void stNodeVisitor::operator()(BinaryOperation& node)
{
    visit(getVariant(node.left()));
    visit(getVariant(node.right()));
}

void stNodeVisitor::operator()(Number&) {}

void stNodeVisitor::operator()(UnaryOp& node)
{
    visit(getVariant(node.expr()));
}

void stNodeVisitor::operator()(Section& node)
{
    for (const auto& singleNode : node.children()) {
        visit(getVariant(singleNode));
    }
}

void stNodeVisitor::operator()(Assign& node)
{
    std::shared_ptr<Variable> variable = std::dynamic_pointer_cast<Variable>(node.left());

    const std::string variableName = variable->name();

    if (st.find(variableName) == nullptr) {
        const std::string errorMessage = "Variable '" + variableName + "' has not been exists";
        throw std::runtime_error(errorMessage);
    }

    visit(getVariant(node.right()));
}

void stNodeVisitor::operator()(VariableDeclaration& node)
{
    std::shared_ptr<Variable> variable = std::dynamic_pointer_cast<Variable>(node.variable().value());

    const std::string variableName = variable->name();

    if (st.find(variableName)) {
        const std::string errorMessage = "Variable '" + variableName + "' already exists";
        throw std::runtime_error(errorMessage);
    }

    const std::variant<int, float, bool> variableValue = variable->token().getFlexNumber();
    std::shared_ptr<st::Symbol> variableType = st.find(Token::typeToString(variable->token().getType()));

    st.create(std::make_shared<st::VariableSymbol>(variableName, variableValue, variableType));
}

void stNodeVisitor::operator()(Variable& variable)
{
    const std::string variableName = variable.name();

    const auto symbol = st.find(variableName);
    if (symbol == nullptr) {
        const std::string errorMessage = "Variable '" + variableName + "' has not been declared";
        throw std::runtime_error(errorMessage);
    }

    if (std::dynamic_pointer_cast<st::VariableSymbol>(symbol) == nullptr) {
        const std::string errorMessage = "'" + variableName + "' is not a name of a variable";
        throw std::runtime_error(errorMessage);
    }
}

void stNodeVisitor::operator()(EmptyNode&) {}

void stNodeVisitor::operator()(FunDeclaration& node)
{
    const std::string functionName = node.name();

    if (st.find(functionName)) {
        const std::string errorMessage = "Function '" + functionName + "' already exists";
        throw std::runtime_error(errorMessage);
    }

    st.create(std::make_shared<st::FunctionSymbol>(functionName, node.body()));
}

void stNodeVisitor::operator()(FunCall& node)
{
    const std::string functionName = node.name();

    auto symbol = st.find(functionName);

    if (symbol == nullptr) {
        const std::string errorMessage = "Function '" + functionName + "' has not been declared";
        throw std::runtime_error(errorMessage);
    }

    if (std::dynamic_pointer_cast<st::FunctionSymbol>(symbol) == nullptr) {
        const std::string errorMessage = "'" + functionName + "' is not a name of a function";
        throw std::runtime_error(errorMessage);
    }
}

void stNodeVisitor::operator()(IfStatement& node)
{
    visit(getVariant(node.condition()));
    visit(getVariant(node.thenBranch()));
    visit(getVariant(node.elseBranch()));
}

void stNodeVisitor::operator()(WhileLoop& node)
{
    visit(getVariant(node.condition()));
    visit(getVariant(node.body()));
}

void stNodeVisitor::operator()(ForLoop& node)
{
    visit(getVariant(node.initialization()));
    visit(getVariant(node.condition()));
    visit(getVariant(node.increment()));
    visit(getVariant(node.body()));
}

void SymbolTableVisitNode::visit(NodeVariant astNode)
{
    return std::visit(SymbolTableVisitNode(st), astNode);
}

std::shared_ptr<AstNode> NodeVisitor::operator()(BinaryOperation& node)
{

    NodeVariant left = getVariant(node.left());
    NodeVariant right = getVariant(node.right());

    TokenType tokenType = node.binaryOperator().getType();
    auto arithmeticIt = std::find(Token::arithmeticTokenTypes.begin(), Token::arithmeticTokenTypes.end(), tokenType);
    if (arithmeticIt != Token::arithmeticTokenTypes.end()) {
        FlexNumber value = calculateBinaryResult(symbolTable(), visitInterpret(left, symbolTable()), visitInterpret(right, symbolTable()), tokenType);
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
        FlexNumber value = calculateBinaryResult(symbolTable(), visitInterpret(left, symbolTable()), visitInterpret(right, symbolTable()), tokenType);
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

std::shared_ptr<AstNode> NodeVisitor::operator()(Number& node)
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

std::shared_ptr<AstNode> NodeVisitor::operator()(UnaryOp& node)
{
    NodeVariant exprVariant = getVariant(node.expr());
    auto res = visitInterpret(exprVariant, symbolTable());

    auto number = std::dynamic_pointer_cast<Number>(res);
    if (!number) {
        throw std::runtime_error("Casting failed in UnaryOp visitInterpret");
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
            throw std::runtime_error("Unknown operator type for int value in UnaryOp visitInterpret");
        }
    } else {
        float floatValue = std::get<float>(value);
        if (type == TokenType::PLUS) {
            return std::make_shared<Number>(Token(floatValue, TokenType::FLOATING_NUMBER));
        } else if (type == TokenType::MINUS) {
            return std::make_shared<Number>(Token(-floatValue, TokenType::FLOATING_NUMBER));
        } else {
            throw std::runtime_error("Unknown operator type for float value in UnaryOp visitInterpret");
        }
    }
}

std::shared_ptr<AstNode> NodeVisitor::operator()(Section& node)
{
    for (const auto& child : node.children()) {
        Interpreter::visitInterpret(getVariant(child), symbolTable());
    }

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> NodeVisitor::operator()(Assign& node)
{
    auto variable = std::dynamic_pointer_cast<Variable>(node.left());
    if (!variable) {
        throw std::runtime_error("Casting to Variable in VisitAssign failed!");
    }

    auto value = std::dynamic_pointer_cast<Number>(visitInterpret(getVariant(node.right()), symbolTable()));
    if (!value) {
        throw std::runtime_error("Casting to Number in VisitAssign failed!");
    }

    std::shared_ptr<st::Symbol> symbol = st.find(variable->name());
    if (symbol == nullptr) {
        throw std::runtime_error("Cannot find value in Assign visit!");
    }

    auto variableSymbol = std::dynamic_pointer_cast<st::VariableSymbol>(symbol);
    if (!variableSymbol) {
        throw std::runtime_error("Casting to VariableSymbol in VisitAssign failed!");
    }

    auto variableSymbolType = std::dynamic_pointer_cast<st::BuiltInTypeSymbol>(variableSymbol->variableType);
    if (!variableSymbolType) {
        throw std::runtime_error("Casting to variableSymbolType in VisitAssign failed!");
    }

    std::cout << "right() node type in visit Assign: " << static_cast<uint8_t>(node.right()->nodeType()) << std::endl;

    std::string valueTypeName;
    auto flexValue = value->value();

    if (std::holds_alternative<int>(flexValue)) {
        valueTypeName = "INTEGER";
    } else if (std::holds_alternative<float>(flexValue)) {
        valueTypeName = "FLOATING_NUMBER";
    } else {
        valueTypeName = "BOOL_VALUE";
    }

    if (variableSymbolType->name() != valueTypeName) {
        throw std::runtime_error("Type mismatch");
    }

    variableSymbol->getValue() = value->value();

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> NodeVisitor::operator()(VariableDeclaration& /*node*/)
{
    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> NodeVisitor::operator()(Variable& node)
{
    const std::string& variableName = node.name();
    auto symbol = symbolTable().findWithType(variableName, st::SymbolType::VARIABLE_SYMBOL);
    auto variableSymbol = std::dynamic_pointer_cast<st::VariableSymbol>(symbol);

    std::variant<int, float, bool>& value = variableSymbol->getValue();

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

std::shared_ptr<AstNode> NodeVisitor::operator()(EmptyNode&)
{
    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> NodeVisitor::operator()(IfStatement& node)
{
    auto conditionResult = visitInterpret(getVariant(node.condition()), symbolTable());
    auto conditionValue = std::dynamic_pointer_cast<Number>(conditionResult);

    if (!conditionValue) {
        throw std::runtime_error("Condition expression in if-statement did not return a Number.");
    }

    FlexNumber condition = conditionValue->value();
    bool conditionIsTrue = (std::holds_alternative<bool>(condition) && std::get<bool>(condition)) // It is usual flow for comparisons
                           || (std::holds_alternative<int>(condition) && std::get<int>(condition) != 0)
                           || (std::holds_alternative<float>(condition) && std::get<float>(condition) != 0.0f);

    if (conditionIsTrue) {
        return visitInterpret(getVariant(node.thenBranch()), symbolTable());
    } else if (node.elseBranch()) {
        return visitInterpret(getVariant(node.elseBranch()), symbolTable());
    } else {
        return std::make_shared<EmptyNode>();
    }
}

std::shared_ptr<AstNode> NodeVisitor::operator()(FunDeclaration& /*node*/)
{
    // const std::string& functionName = node.name();
    // auto symbol = symbolTable().findWithType(functionName, st::SymbolType::FUNCTION_SYMBOL);
    // auto functionSymbol = std::dynamic_pointer_cast<st::FunctionSymbol>(symbol);

    // std::variant<int, float, bool>& value = functionSymbol->getValue();

    // GLOBAL_FUNCTIONS[node.name()] = node.body();

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> NodeVisitor::operator()(FunCall& node)
{
    const std::string& functionName = node.name();
    auto symbol = symbolTable().findWithType(functionName, st::SymbolType::FUNCTION_SYMBOL);
    auto functionSymbol = std::dynamic_pointer_cast<st::FunctionSymbol>(symbol);

    const auto& functionBody = functionSymbol->body();
    for (const auto& statement : functionBody) {
        visitInterpret(getVariant(statement), symbolTable());
    }

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> NodeVisitor::operator()(WhileLoop& node)
{
    while (true) {
        auto conditionResult = visitInterpret(getVariant(node.condition()), symbolTable());
        auto conditionValue = std::dynamic_pointer_cast<Number>(conditionResult);

        if (!conditionValue) {
            throw std::runtime_error("Condition expression in while-loop did not return a Number.");
        }

        bool conditionIsTrue = (std::holds_alternative<int>(conditionValue->value()) && std::get<int>(conditionValue->value()) != 0)
                               || (std::holds_alternative<float>(conditionValue->value()) && std::get<float>(conditionValue->value()) != 0.0f);

        if (!conditionIsTrue) {
            break;
        }

        visitInterpret(getVariant(node.body()), symbolTable());
    }

    return std::make_shared<EmptyNode>();
}

std::shared_ptr<AstNode> NodeVisitor::operator()(ForLoop& node)
{
    visitInterpret(getVariant(node.initialization()), symbolTable());

    while (true) {
        auto conditionResult = visitInterpret(getVariant(node.condition()), symbolTable());
        auto conditionValue = std::dynamic_pointer_cast<Number>(conditionResult);

        if (!conditionValue) {
            throw std::runtime_error("Condition expression in for-loop did not return a Number.");
        }

        bool conditionIsTrue = (std::holds_alternative<int>(conditionValue->value()) && std::get<int>(conditionValue->value()) != 0)
                               || (std::holds_alternative<float>(conditionValue->value()) && std::get<float>(conditionValue->value()) != 0.0f);

        if (!conditionIsTrue) {
            break;
        }

        visitInterpret(getVariant(node.body()), symbolTable());

        visitInterpret(getVariant(node.increment()), symbolTable());
    }

    return std::make_shared<EmptyNode>();
}

// static
std::shared_ptr<AstNode> Interpreter::visitInterpret(NodeVariant astNode, st::SymbolTable& st)
{
    return std::visit(VisitNode(st), astNode);
}

std::shared_ptr<AstNode> Interpreter::buildTree(const std::string& text)
{
    reset();

    _parser = Parser(Lexer(text));

    return _parser.parse();
}

std::shared_ptr<AstNode> Interpreter::interpret(std::shared_ptr<AstNode> tree)
{
    return Interpreter::visitInterpret(getVariant(tree), symbolTable());
}

void Interpreter::reset()
{
    GLOBAL_SCOPE.clear();
    GLOBAL_FUNCTIONS.clear();

    _executionLine = -1;
    _executionPosition = -1;

    _parser = Parser();
}

// static
FlexNumber Interpreter::calculateBinaryResult(st::SymbolTable& st,
                                              const std::shared_ptr<AstNode>& left,
                                              const std::shared_ptr<AstNode>& right,
                                              TokenType type)
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

        auto symbol = st.findWithType(leftVariable->name(), st::SymbolType::VARIABLE_SYMBOL);
        auto variableSymbol = std::dynamic_pointer_cast<st::VariableSymbol>(symbol);

        FlexNumber leftValue = variableSymbol->getValue();

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
