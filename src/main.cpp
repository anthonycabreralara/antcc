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

void compileAsmFile(const std::string& filename) {
    std::string cmd = "gcc " + filename + ".s -o " + filename;

    int result = system(cmd.c_str());

    if (result == 0) {
        std::cout << "Compilation succeeded.\n";
    } else {
        std::cout << "Compilation failed with code " << result << ".\n";
    }
}

std::string getFileName(const std::string filename) {
    std::string segment;
    std::vector<std::string> segments;
    std::istringstream ss(filename); // Create a stringstream from the input string

    while (std::getline(ss, segment, '/')) { // Read segments separated by ','
        segments.push_back(segment);
    }
    
    std::string res = segments[segments.size() - 1];
    
    if (res.size() >= 2 && res.substr(res.size() - 2) == ".c") {
        res = res.substr(0, res.size() - 2);
    }

    return res;
}

int main(int argc, char *argv[]) {
    std::string option = "";
    std::string sourceCode;
    std::string filename = "";

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
        filename = getFileName(sourceCodeFilepath);
    }
    // Too many args
    else {
        std::cout << "Too many arguments." << std::endl;
        return 1;
    }

    // --- Compiler pipeline ---
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    if (!lexer.valid) {
        std::cout << "Invalid token(s):" << std::endl;
        for (const auto &token: tokens) {
            if (token.type == TokenType::UNKNOWN) {
                std::cout << token.value << std::endl;
            }
        }
        return 1;
    }

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

    emitCode(asm_ir.get(), filename);
    compileAsmFile(filename);
    

    return 0;
}