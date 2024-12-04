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

        interpreter.symbolTable().init();
        interpreter.initParser(input);

        interpreter.registerBuiltInMethod("moveNorth()");
        interpreter.registerBuiltInMethod("moveSouth()");
        interpreter.registerBuiltInMethod("moveEast()");
        interpreter.registerBuiltInMethod("moveWest()");
        interpreter.registerBuiltInMethod("rotateLeft()");
        interpreter.registerBuiltInMethod("rotateRight()");
        interpreter.registerBuiltInMethod("push()");

        std::shared_ptr<AstNode> tree = interpreter.buildTree(input);

        st::SymbolTable& stRef = interpreter.symbolTable();
        SymbolTableBuilder stb;
        stb.build(tree, stRef);

        std::shared_ptr<AstNode> result = interpreter.interpret(tree);

        stRef.debugPrint();

        return 0;
    }

    return 1;
}

// #include <algorithm>
// #include <cmath>
// #include <cstdint>
// #include <iostream>
// #include <set>

// uint64_t hamber(uint64_t n)
// {
//     std::set<uint64_t> numbers;

//     uint64_t prevThresh = 0;
//     uint64_t threshold = 3;
//     uint64_t x = 0;
//     uint64_t y = 0;
//     uint64_t z = 0;

//     uint64_t result = 0;
//     while (numbers.size() < n) {
//         x = prevThresh;
//         y = prevThresh;
//         z = prevThresh;
//         for (; z < threshold; ++z) {
//             y = prevThresh;
//             x = prevThresh;
//             for (; y < threshold; ++y) {
//                 x = prevThresh;
//                 for (; x < threshold; ++x) {
//                     result = pow(2, x) * pow(3, y) * pow(5, z);
//                     std::cout << x << " " << y << " " << z << " = " << result << std::endl;
//                     numbers.insert(result);
//                 }
//             }
//         }

//         x = y = z = threshold;
//         prevThresh += 2;
//         threshold += 2;
//     }

//     auto it = numbers.begin();
//     for (int i = 0; i < n; ++i) {
//         it++;
//     }

//     it--;

//     return *it;
// }

// int main(int, char* argv[])
// {
//     uint64_t arg = std::stoi(argv[1]);
//     std::cout << hamber(arg) << std::endl;
//     return 0;
// }