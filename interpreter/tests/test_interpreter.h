#pragma once

#include "../CodeInterpreter.h"
#include "tests/test_macros.h"

namespace TestInterpreter {

TEST_CASE("[Interpreter] 2+2")
{
    CodeInterpreter interpreter{};
    interpreter.interpret("START auto val1 = 2 + 2 END.");

    CHECK(interpreter.getVariable("val1") == "4");
}

} // namespace TestInterpreter
