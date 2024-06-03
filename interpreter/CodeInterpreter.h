#ifndef CodeInterpreter_H
#define CodeInterpreter_H


#include "internal/Interpreter.h"
#include "core/object/ref_counted.h"

class CodeInterpreter : public RefCounted, public Interpreter {
    GDCLASS(CodeInterpreter, RefCounted);

protected:
    static void _bind_methods();
public:
    String interpret(const String& str);
};

#endif