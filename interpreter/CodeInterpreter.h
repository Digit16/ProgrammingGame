#pragma once

#include "core/object/ref_counted.h"
#include "core/variant/dictionary.h"
#include "src/Interpreter.h"

class CodeInterpreter : public RefCounted, public Interpreter
{
    GDCLASS(CodeInterpreter, RefCounted);

protected:
    static void _bind_methods();

public:
    void registerBuiltInMethod(const String& str);
    String interpret(const String& str);
    String getVariable(const String& str);
    Array getSequence();

private:
    Interpreter interpreter;
};
