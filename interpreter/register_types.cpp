/* register_types.cpp */

#include "register_types.h"

#include "core/object/class_db.h"
#include "CodeInterpreter.h"

void initialize_interpreter_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
			return;
	}
	GDREGISTER_CLASS(CodeInterpreter);
}

void uninitialize_interpreter_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
			return;
	}
   // Nothing to do here in this example.
}