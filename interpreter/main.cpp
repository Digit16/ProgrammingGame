#include "./src/Ast.h"
#include "./src/Interpreter.h"
#include "./src/Lexer.h"
#include "./src/Parser.h"
#include "./src/Token.h"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{

    if (argc > 1) {
        std::string input;

        std::string file_path;
        std::string other_arg;

        std::string arg = argv[1];

        if (arg.find("--file=") == 0) {
            file_path = arg.substr(7);
        } else {
            other_arg = arg;
        }

        if (!file_path.empty()) {
            std::cout << "File path: " << file_path << std::endl;

            std::ifstream file(file_path);

            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                input = buffer.str();

                std::cout << "File content:\n" << input << std::endl;
            } else {
                std::cerr << "Error: Could not open file " << file_path << std::endl;
            }

        } else {
            std::cout << "No file path provided. Use --file=path_to_file" << std::endl;
            if (!arg.empty()) {
                std::cout << "Argument:" << std::endl;
                std::cout << arg << std::endl;
                input = arg;
            }
        }

        Interpreter interpreter;
        std::shared_ptr<AstNode> tree = interpreter.buildTree(input);

        st::SymbolTable& stRef = interpreter.symbolTable();
        SymbolTableBuilder stb;
        stb.build(tree, stRef);

        std::shared_ptr<AstNode> result = interpreter.interpret(tree);

        // std::cout << "Variables:" << std::endl;

        // for (const auto& [name, value] : GLOBAL_SCOPE) {
        //     if (std::holds_alternative<int>(value)) {
        //         std::cout << "[" << name << " = " << std::get<int>(value) << "]" << std::endl;
        //     } else {
        //         std::cout << "[" << name << " = " << std::get<float>(value) << "]" << std::endl;
        //     }
        // }

        std::cout << "Result node type: " << getTypeString(result->nodeType()) << std::endl;

        return 0;
    }

    return 1;
}
