#include <iostream>
#include <sstream>

#include "CodeInterpreter.h"

void CodeInterpreter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("interpret"), &CodeInterpreter::interpret);   
	ClassDB::bind_method(D_METHOD("getVariable"), &CodeInterpreter::getVariable);  
}

void CodeInterpreter::interpret(const String& str) {
    std::string text{str.utf8().get_data()};
    //std::shared_ptr<AstNode> result = Interpreter::interpret(text);
	
	std::shared_ptr<AstNode> tree = interpreter.buildTree(text);

	st::SymbolTable& stRef = interpreter.symbolTable();
	SymbolTableBuilder stb;
	stb.build(tree, stRef);

	std::shared_ptr<AstNode> result = interpreter.interpret(tree);
}

String CodeInterpreter::getVariable(const String& str) {
	std::string text{str.utf8().get_data()};
	std::stringstream ssOut;
	
	using namespace st;
	
	try {
		auto variable = interpreter.symbolTable().findWithType(text, SymbolType::VARIABLE_SYMBOL); // TODO: Handle exception
		auto variableSymbol = std::dynamic_pointer_cast<VariableSymbol>(variable);
		
		std::variant<int, float, bool> value = variableSymbol->getValue();
		
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