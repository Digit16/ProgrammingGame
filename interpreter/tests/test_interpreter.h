#ifndef TEST_INTERPRETER_H
#define TEST_INTERPRETER_H

#include "tests/test_macros.h"

#include "../CodeInterpreter.h"

namespace TestInterpreter {

TEST_CASE("[Interpreter] 2+2") {
	CodeInterpreter interpreter{};
	String rv = interpreter.interpret("2+2");
	CHECK(rv == "4");
}

} 

#endif // TEST_INTERPRETER_H