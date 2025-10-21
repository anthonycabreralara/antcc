#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "generate_tacky.h"
#include "emitter.h"
#include "ast.h"
#include <iostream>
#include <string>
#include <sstream>
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
    std::string sourceCode;

    // Case 1: Read from stdin (no filepath argument)
    if (argc == 1) {
        std::ostringstream ss;
        ss << std::cin.rdbuf();  // read all of stdin into string
        sourceCode = ss.str();
    }
    // Case 2: Filepath only or with option
    else if (argc == 2 || argc == 3) {
        std::string sourceCodeFilepath = argv[1];

        if (argc == 3) {
            option = argv[2];
            if (option != "--lex" && option != "--parse" && option != "--codegen" && option != "--tacky") {
                std::cout << "Invalid options." << std::endl;
                return 1;
            }
        }

        sourceCode = readFileToString(sourceCodeFilepath);
    }
    // Too many args
    else {
        std::cout << "Too many arguments." << std::endl;
        return 1;
    }

    // --- Compiler pipeline ---
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    if (option == "--lex") {
        printTokens(tokens);
        return 0;
    }

    Parser parser(tokens);
    auto ast = parser.parse();

    if (option == "--parse") {
        printAST(ast.get(), 0);
        return 0;
    }

    auto tacky_ir = generateTacky(ast.get(), nullptr);
    if (option == "--tacky") {
        printTacky(tacky_ir.get(), 0);
        return 0;
    }

    
    auto asm_ir = generateCode(tacky_ir.get());
    if (option == "--codegen") {
        printIR(asm_ir.get(), 0);
        return 0;
    }

    //emitCode(asm_ir.get());
    

    return 0;
}

