#pragma once


#include "tests/test_macros.h"
#include "../CodeInterpreter.h"

namespace TestInterpreter {


TEST_CASE("[Interpreter] 2+2") {
	CodeInterpreter interpreter{};
	interpreter.interpret("START auto val1 = 2 + 2 END.");
	
	CHECK(interpreter.getVariable("val1") == "4");
}


} 
