#pragma once

#include "src/Interpreter.h"
#include "core/object/ref_counted.h"
#include "core/variant/dictionary.h"

class CodeInterpreter : public RefCounted, public Interpreter {
    GDCLASS(CodeInterpreter, RefCounted);

protected:
    static void _bind_methods();
public:
    void interpret(const String& str);
	String getVariable(const String& str);
	Dictionary getGlobalScope();
	
};
