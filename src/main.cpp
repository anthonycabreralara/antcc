#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "generate_tacky.h"
#include "emitter.h"
#include "ast.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <memory>
#include <array>

std::string readFileToString(const std::string &filename) {
    std::string cmd = "gcc -E -P " + filename; // or "clang -E -P " + filename
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

int main(int argc, char *argv[]) {
    std::string option = "";
    if (argc < 2) {
        std::cout << "Need filepath." << std::endl;
        return 1;
    } else if (argc > 3) {
        std::cout << "Too many arguments." << std::endl;
        return 1;
    } else if (argc == 3) {
        option = argv[2];
        if (option != "--lex" && option != "--parse" && option != "--codegen" && option != "--tacky") {
            std::cout << "Invalid options." << std::endl;
            return 1;
        }
    }

    std::string sourceCodeFilepath = argv[1];

    std::string sourceCode = readFileToString(sourceCodeFilepath);

    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    if (option == "--lex") {
        printTokens(tokens);
        return 0;
    }

    if (!lexer.isValid()) {
        std::cout << "Invalid source code." << std::endl;
        return 1;
    }

    Parser parser(tokens);
    auto ast = parser.parse();  // unique_ptr<Node>

    if (option == "--parse") {
        printAST(ast.get(), 0);  // pass raw pointer for printing
        return 0;
    }


    auto tacky_ir = generateTacky(ast.get(), nullptr);
    if (option == "--tacky") {
        printTacky(tacky_ir.get(), 0);
        return 0;
    }

    auto asm_ir = generateCode(ast.get());
    if (option == "--codegen") {
        printIR(asm_ir.get(), 0);
        return 0;
    }

    emitCode(asm_ir.get());





    return 0;
}
