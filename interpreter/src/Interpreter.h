#pragma once

#include "Ast.h"
#include "Lexer.h"
#include "Parser.h"

#include <optional>
#include <unordered_map>
#include <variant>

namespace st {
enum class SymbolVariableType : uint8_t
{
    INTEGER = 0,
    FLOATING_NUMBER = 1,
    BOOL_VALUE = 2
};

enum class SymbolType : uint8_t
{
    SYMBOL = 0,
    BUILT_IN_TYPE_SYMBOL = 1,
    VARIABLE_SYMBOL = 2,
    FUNCTION_SYMBOL = 3
};

class Symbol
{
public:
    Symbol(const std::string& name) :
        _name(name)
    {
    }

    std::string name() const { return _name; }

    virtual SymbolType symbolType() { return SymbolType::SYMBOL; }

    // protected:
    std::string _name;
};

class BuiltInTypeSymbol : public Symbol
{
public:
    BuiltInTypeSymbol(const std::string& symbolName, SymbolVariableType type) :
        Symbol(symbolName),
        _type(type)
    {
    }

    SymbolType symbolType() override { return SymbolType::BUILT_IN_TYPE_SYMBOL; }

    // protected:
    SymbolVariableType _type;
};

class VariableSymbol : public Symbol
{
public:
    VariableSymbol(const std::string& name, const std::variant<int, float, bool>& value, std::shared_ptr<Symbol> type) :
        Symbol(name),
        variableValue(value),
        variableType(type)
    {
    }

    SymbolType symbolType() override { return SymbolType::VARIABLE_SYMBOL; }

    std::variant<int, float, bool>& getValue() { return variableValue; }

    std::variant<int, float, bool> variableValue;
    std::shared_ptr<Symbol> variableType;
};

class FunctionSymbol : public Symbol
{
public:
    FunctionSymbol(const std::string& fName, std::vector<std::shared_ptr<AstNode>> function) :
        Symbol(fName),
        _function(function)
    {
    }

    SymbolType symbolType() override { return SymbolType::FUNCTION_SYMBOL; }

    std::vector<std::shared_ptr<AstNode>>& body() { return _function; }

    // protected:
    // std::string _functionName;
    std::vector<std::shared_ptr<AstNode>> _function;
};

// class FunctionCallSymbol : public Symbol
// {
// protected:
//     std::shared_ptr<Symbol> _functionToCall;
// };

class SymbolTable
{
public:
    void create(const std::shared_ptr<Symbol>& symbol);

    std::shared_ptr<Symbol> find(const std::string& symbolName);

    std::shared_ptr<Symbol> findWithType(const std::string& symbolName, SymbolType type);

    void init();

    void debugPrint();

private:
    std::map<std::string, std::shared_ptr<Symbol>> _symbols;
};

} // namespace st

// class InterpreterNodeVisitor
// {
// public:
//     struct VisitNode
//     {
//         VisitNode(st::SymbolTable& st) :
//             _st(st)
//         {
//         }
//         ~VisitNode() = default;

//         std::shared_ptr<AstNode> operator()(BinaryOperation& node);
//         std::shared_ptr<AstNode> operator()(Number& node);
//         std::shared_ptr<AstNode> operator()(UnaryOp& node);
//         std::shared_ptr<AstNode> operator()(Section& node);
//         std::shared_ptr<AstNode> operator()(Assign& node);
//         std::shared_ptr<AstNode> operator()(VariableDeclaration& node);
//         std::shared_ptr<AstNode> operator()(Variable& node);
//         std::shared_ptr<AstNode> operator()(EmptyNode& node);
//         std::shared_ptr<AstNode> operator()(FunDeclaration& node);
//         std::shared_ptr<AstNode> operator()(FunCall& node);
//         std::shared_ptr<AstNode> operator()(IfStatement& node);
//         std::shared_ptr<AstNode> operator()(WhileLoop& node);
//         std::shared_ptr<AstNode> operator()(ForLoop& node);

//         st::SymbolTable& _st;
//     };

//     static std::shared_ptr<AstNode> visitInterpret(NodeVariant astNode);

//     static int calculateUnaryResult(const std::shared_ptr<AstNode>& node, TokenType op);
//     static FlexNumber calculateBinaryResult(const std::shared_ptr<AstNode>& left, const std::shared_ptr<AstNode>& right, TokenType type);
// };

class Interpreter /* : public InterpreterNodeVisitor*/
{
public:
    std::shared_ptr<AstNode> buildTree(const std::string& text);

    std::shared_ptr<AstNode> interpret(std::shared_ptr<AstNode> tree);

    void reset();

    std::map<std::string, std::variant<int, float, bool>>& getGlobalScope() { return GLOBAL_SCOPE; }
    std::unordered_map<std::string, std::vector<std::shared_ptr<AstNode>>>& getGlobalFunctions() { return GLOBAL_FUNCTIONS; }

    void raiseExecutionError(uint16_t currentLine, int16_t currentPositon = -1) const;

    st::SymbolTable& symbolTable() { return _symbolTable; }

    struct VisitNode
    {
        VisitNode(st::SymbolTable& st) :
            st(st)
        {
        }
        ~VisitNode() = default;

        std::shared_ptr<AstNode> operator()(BinaryOperation& node);
        std::shared_ptr<AstNode> operator()(Number& node);
        std::shared_ptr<AstNode> operator()(UnaryOp& node);
        std::shared_ptr<AstNode> operator()(Section& node);
        std::shared_ptr<AstNode> operator()(Assign& node);
        std::shared_ptr<AstNode> operator()(VariableDeclaration& node);
        std::shared_ptr<AstNode> operator()(Variable& node);
        std::shared_ptr<AstNode> operator()(EmptyNode& node);
        std::shared_ptr<AstNode> operator()(FunDeclaration& node);
        std::shared_ptr<AstNode> operator()(FunCall& node);
        std::shared_ptr<AstNode> operator()(IfStatement& node);
        std::shared_ptr<AstNode> operator()(WhileLoop& node);
        std::shared_ptr<AstNode> operator()(ForLoop& node);

        st::SymbolTable& symbolTable() { return st; }

        st::SymbolTable& st;
    };

    static std::shared_ptr<AstNode> visitInterpret(NodeVariant astNode, st::SymbolTable& st);

    static int calculateUnaryResult(const std::shared_ptr<AstNode>& node, TokenType op);
    static FlexNumber calculateBinaryResult(st::SymbolTable& st,
                                            const std::shared_ptr<AstNode>& left,
                                            const std::shared_ptr<AstNode>& right,
                                            TokenType type);

private:
    st::SymbolTable _symbolTable;
    std::map<std::string, std::variant<int, float, bool>> GLOBAL_SCOPE;
    std::unordered_map<std::string, std::vector<std::shared_ptr<AstNode>>> GLOBAL_FUNCTIONS;

    uint16_t _executionPosition;
    uint16_t _executionLine;

    Parser _parser;
};

class SymbolTableBuilder
{
public:
    void build(std::shared_ptr<AstNode> tree, st::SymbolTable& st);

    //     st::SymbolTable& symbolTable() { return _symbolTable; }

    // protected:
    //     st::SymbolTable& _symbolTable;
};

class SymbolTableVisitNode
{
public:
    SymbolTableVisitNode(st::SymbolTable& st) :
        st(st)
    {
    }
    ~SymbolTableVisitNode() = default;

    void operator()(BinaryOperation& node /*, st::SymbolTable& st*/);
    void operator()(Number& node /*, st::SymbolTable& st*/);
    void operator()(UnaryOp& node /*, st::SymbolTable& st*/);
    void operator()(Section& node /*, st::SymbolTable& st*/);
    void operator()(Assign& node /*, st::SymbolTable& st*/);
    void operator()(VariableDeclaration& node /*, st::SymbolTable& st*/);
    void operator()(Variable& node /*, st::SymbolTable& st*/);
    void operator()(EmptyNode& node /*, st::SymbolTable& st*/);
    void operator()(FunDeclaration& node /*, st::SymbolTable& st*/);
    void operator()(FunCall& node /*, st::SymbolTable& st*/);
    void operator()(IfStatement& node /*, st::SymbolTable& st*/);
    void operator()(WhileLoop& node /*, st::SymbolTable& st*/);
    void operator()(ForLoop& node /*, st::SymbolTable& st*/);

    void visit(NodeVariant astNode /*, st::SymbolTable& st*/);

    std::variant<int, float, bool> evaluateExpression(const std::shared_ptr<AstNode> node);
    std::string getTokenTypeFromVariant(std::variant<int, float, bool> variant);

    st::SymbolTable& st;
};
