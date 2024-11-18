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
    BUILT_IN_FUNCTION_SYMBOL = 2,
    VARIABLE_SYMBOL = 3,
    FUNCTION_SYMBOL = 4
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

    SymbolVariableType _type;
};

class BuiltInFunctionSymbol : public Symbol
{
public:
    BuiltInFunctionSymbol(const std::string& symbolName) :
        Symbol(symbolName)
    {
    }

    SymbolType symbolType() override { return SymbolType::BUILT_IN_TYPE_SYMBOL; }
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

    std::vector<std::shared_ptr<AstNode>> _function;
};

class SymbolTable
{
public:
    void create(const std::shared_ptr<Symbol>& symbol);

    std::shared_ptr<Symbol> find(const std::string& symbolName);

    std::shared_ptr<Symbol> findWithType(const std::string& symbolName, SymbolType type);

    void init();

    void registerBuiltInMethod(const std::string& functionName);

    void addToSequence(const std::string& functionName);

    std::vector<std::string> sequence();

    void debugPrint();

private:
    std::map<std::string, std::shared_ptr<Symbol>> _symbols;
    std::vector<std::string> _sequence;
};

} // namespace st

class Interpreter
{
public:
    void registerBuiltInMethod(const std::string& functionName);

    std::vector<std::string> getSequence();

    std::shared_ptr<AstNode> buildTree();

    std::shared_ptr<AstNode> interpret(std::shared_ptr<AstNode> tree);

    void initParser(const std::string& input);

    void reset();

    std::variant<int, float, bool> getVariableVariant(const std::string& varialeName);

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
        std::shared_ptr<AstNode> operator()(BuiltInFunction& node);
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

    uint16_t _executionPosition;
    uint16_t _executionLine;

    Parser _parser;
};

class SymbolTableBuilder
{
public:
    void build(std::shared_ptr<AstNode> tree, st::SymbolTable& st);
};

class SymbolTableVisitNode
{
public:
    SymbolTableVisitNode(st::SymbolTable& st) :
        st(st)
    {
    }
    ~SymbolTableVisitNode() = default;

    void operator()(BinaryOperation& node);
    void operator()(Number& node);
    void operator()(UnaryOp& node);
    void operator()(Section& node);
    void operator()(Assign& node);
    void operator()(VariableDeclaration& node);
    void operator()(Variable& node);
    void operator()(EmptyNode& node);
    void operator()(FunDeclaration& node);
    void operator()(BuiltInFunction& node);
    void operator()(FunCall& node);
    void operator()(IfStatement& node);
    void operator()(WhileLoop& node);
    void operator()(ForLoop& node);

    void visit(NodeVariant astNode);

    std::variant<int, float, bool> evaluateExpression(const std::shared_ptr<AstNode> node);
    std::string getTokenTypeFromVariant(std::variant<int, float, bool> variant);

    st::SymbolTable& st;
};
