#ifndef ASM_IR_H
#define ASM_IR_H

#include <string>
#include <vector>
#include <memory>

enum class AsmIRNodeType { PROGRAM, FUNCTION, MOV, IMMEDIATE, RETURN, REGISTER, INSTRUCTIONS };

class AsmIRNode {
public:
    AsmIRNodeType type;
    virtual ~AsmIRNode() = default;
};

class AsmIRRet : public AsmIRNode {
public:
    AsmIRRet();
};

class AsmIRImm : public AsmIRNode {
public:
    std::string value;
    AsmIRImm(std::string v);
};

class AsmIRMov : public AsmIRNode {
public:
    std::unique_ptr<AsmIRNode> src;
    std::unique_ptr<AsmIRNode> dst;
    AsmIRMov(std::unique_ptr<AsmIRNode> s, std::unique_ptr<AsmIRNode> d);
};

class AsmIRReg : public AsmIRNode {
public:
    std::string value;
    AsmIRReg(std::string v);
};

class AsmIRInstructions : public AsmIRNode {
public:
    std::vector<std::unique_ptr<AsmIRNode>> instructions;

    AsmIRInstructions();
};

class AsmIRFunction : public AsmIRNode {
public:
    std::string name;
    std::unique_ptr<AsmIRInstructions> instructions;

    AsmIRFunction(std::string n, std::unique_ptr<AsmIRInstructions> instr);
};

class AsmIRProgram : public AsmIRNode {
public:
    std::unique_ptr<AsmIRFunction> function;
    AsmIRProgram(std::unique_ptr<AsmIRFunction> func);
};

#endif
