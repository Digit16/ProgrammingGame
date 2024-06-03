#include "CodeInterpreter.h"

void CodeInterpreter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("interpret"), &CodeInterpreter::interpret);   
}

String CodeInterpreter::interpret(const String& str) {
    std::string text{str.utf8().get_data()};

    std::shared_ptr<AstNode> result = Interpreter::interpret(text);
    auto number = std::dynamic_pointer_cast<Number>(result);
    return String(std::to_string(number->value()).c_str());
}