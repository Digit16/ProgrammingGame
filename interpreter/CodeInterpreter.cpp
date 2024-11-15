#include "CodeInterpreter.h"

#include <iostream>
#include <sstream>

void CodeInterpreter::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("registerBuiltInMethod"), &CodeInterpreter::registerBuiltInMethod);
    ClassDB::bind_method(D_METHOD("interpret"), &CodeInterpreter::interpret);
    ClassDB::bind_method(D_METHOD("getVariable"), &CodeInterpreter::getVariable);
    ClassDB::bind_method(D_METHOD("getSequence"), &CodeInterpreter::getSequence);
}

void CodeInterpreter::registerBuiltInMethod(const String& str);
{
    const std::string text{str.utf8().get_data()};

    interpreter.registerBuiltInMethod(text);
}

String CodeInterpreter::interpret(const String& str)
{
    try {
        const std::string text{str.utf8().get_data()};

        interpreter.symbolTable().init();
        interpreter.initParser(input);

        interpreter.registerBuiltInMethod("moveNorth()");
        interpreter.registerBuiltInMethod("moveSouth()");
        interpreter.registerBuiltInMethod("moveEast()");
        interpreter.registerBuiltInMethod("moveWest()");
        interpreter.registerBuiltInMethod("rotateLeft()");
        interpreter.registerBuiltInMethod("rotateRight()");
        interpreter.registerBuiltInMethod("push()");

        std::shared_ptr<AstNode> tree = interpreter.buildTree(text);

        st::SymbolTable& stRef = interpreter.symbolTable();
        SymbolTableBuilder stb;
        stb.build(tree, stRef);

        std::shared_ptr<AstNode> result = interpreter.interpret(tree);

        return String();
    } catch (const std::exception& e) {
        return String(e.what());
    }
}

String CodeInterpreter::getVariable(const String& str)
{
    const std::string variableName{str.utf8().get_data()};
    std::stringstream ssOut;

    try {
        std::variant<int, float, bool> value = interpreter.getVariableVariant(variableName);

        if (std::holds_alternative<int>(value)) {
            int intValue = std::get<int>(value);
            std::cout << "Int value" << intValue << std::endl;
            ssOut << intValue;
        } else if (std::holds_alternative<float>(value)) {
            float floatValue = std::get<float>(value);
            std::cout << "Float value" << floatValue << std::endl;
            ssOut << floatValue;
        } else if (std::holds_alternative<bool>(value)) {
            bool boolValue = std::get<bool>(value);
            std::cout << "Bool value" << boolValue << std::endl;
            ssOut << boolValue;
        } else {
            std::cout << "Unknown variant" << std::endl;
            ssOut << "Unknown variant";
        }

        std::string sOut = ssOut.str();
        return String(sOut.c_str());

    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return String("Error");
    }
}

Array CodeInterpreter::getSequence()
{
    Array array;
    std::vector<std::string> seq = interpreter.getSequence();
    for (const auto& instruction : seq) {
        array.append(String(instruction.c_str()));
    }

    return array;
}
