#include "Ast.h"

NodeVariant getVariant(const std::shared_ptr<AstNode>& node)
{
    switch (node->nodeType()) {
    case NodeType::NUMBER: {
        auto number = std::dynamic_pointer_cast<Number>(node);
        if (!number) {
            throw std::runtime_error("Casting to Number failed");
        }

        return NodeVariant(Number(*number));
    }
    case NodeType::BINARY_OPERATION: {
        auto binOp = std::dynamic_pointer_cast<BinaryOperation>(node);
        if (!binOp) {
            throw std::runtime_error("Casting to BinaryOperation failed");
        }

        return NodeVariant(BinaryOperation(*binOp));
    }
    case NodeType::UNARY_OPERATOR: {
        auto unOp = std::dynamic_pointer_cast<UnaryOp>(node);
        if (!unOp) {
            throw std::runtime_error("Casting to UnaryOp failed");
        }

        return NodeVariant(UnaryOp(*unOp));
    }
    case NodeType::SECTION: {
        auto section = std::dynamic_pointer_cast<Section>(node);
        if (!section) {
            throw std::runtime_error("Casting to Section failed");
        }

        return NodeVariant(Section(*section));
    }
    case NodeType::ASSIGN: {
        auto assign = std::dynamic_pointer_cast<Assign>(node);
        if (!assign) {
            throw std::runtime_error("Casting to Assign failed");
        }

        return NodeVariant(Assign(*assign));
    }
    case NodeType::VARIABLE_DECLARATION: {
        auto variableDeclaration = std::dynamic_pointer_cast<VariableDeclaration>(node);
        if (!variableDeclaration) {
            throw std::runtime_error("Casting to VariableDeclaration failed");
        }

        return NodeVariant(VariableDeclaration(*variableDeclaration));
    }
    case NodeType::VARIABLE: {
        auto variable = std::dynamic_pointer_cast<Variable>(node);
        if (!variable) {
            throw std::runtime_error("Casting to Variable failed");
        }

        return NodeVariant(Variable(*variable));
    }
    case NodeType::EMPTY: {
        auto empty = std::dynamic_pointer_cast<EmptyNode>(node);
        if (!empty) {
            throw std::runtime_error("Casting failed");
        }

        return NodeVariant(EmptyNode(*empty));
    }
    case NodeType::FUN_DECLARATION: {
        auto funDeclaration = std::dynamic_pointer_cast<FunDeclaration>(node);
        if (!funDeclaration) {
            throw std::runtime_error("Casting to FunDeclaration failed");
        }

        return NodeVariant(FunDeclaration(*funDeclaration));
    }
    case NodeType::FUN_CALL: {
        auto funCall = std::dynamic_pointer_cast<FunCall>(node);
        if (!funCall) {
            throw std::runtime_error("Casting to FunCall failed");
        }

        return NodeVariant(FunCall(*funCall));
    }
    case NodeType::IF: {
        auto ifAst = std::dynamic_pointer_cast<IfStatement>(node);
        if (!ifAst) {
            throw std::runtime_error("Casting to IfStatement failed");
        }

        return NodeVariant(IfStatement(*ifAst));
    }
    case NodeType::WHILE: {
        auto whileAst = std::dynamic_pointer_cast<WhileLoop>(node);
        if (!whileAst) {
            throw std::runtime_error("Casting to WhileLoop failed");
        }

        return NodeVariant(WhileLoop(*whileAst));
    }
    case NodeType::FOR: {
        auto forAst = std::dynamic_pointer_cast<ForLoop>(node);
        if (!forAst) {
            throw std::runtime_error("Casting to ForLoop failed");
        }

        return NodeVariant(ForLoop(*forAst));
    }
    }

    throw std::runtime_error("Unknown NodeType");
}

std::string getTypeString(NodeType nt)
{
    switch (nt) {
    case NodeType::ASSIGN:
        return "ASSIGN";
    case NodeType::BINARY_OPERATION:
        return "BINARY_OPERATION";
    case NodeType::EMPTY:
        return "EMPTY";
    case NodeType::NUMBER:
        return "NUMBER";
    case NodeType::SECTION:
        return "SECTION";
    case NodeType::UNARY_OPERATOR:
        return "UNARY_OPERATOR";
    case NodeType::VARIABLE:
        return "VARIABLE";
    case NodeType::FUN_DECLARATION:
        return "FUN_DECLARATION";
    case NodeType::FUN_CALL:
        return "FUN_CALL";
    case NodeType::IF:
        return "IF";
    case NodeType::WHILE:
        return "WHILE";
    case NodeType::FOR:
        return "FOR";
    }

    throw std::runtime_error("Unknown NodeType");
}

FlexNumber operator+(const FlexNumber& lhs, const FlexNumber& rhs)
{
    return std::visit(FlexNumberAdder{}, lhs, rhs);
}

FlexNumber operator-(const FlexNumber& lhs, const FlexNumber& rhs)
{
    return std::visit(FlexNumberSubtractor{}, lhs, rhs);
}

FlexNumber operator*(const FlexNumber& lhs, const FlexNumber& rhs)
{
    return std::visit(FlexNumberMultiplier{}, lhs, rhs);
}

FlexNumber operator/(const FlexNumber& lhs, const FlexNumber& rhs)
{
    return std::visit(FlexNumberDivider{}, lhs, rhs);
}