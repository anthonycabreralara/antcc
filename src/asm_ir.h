#ifndef ASM_IR_H
#define ASM_IR_H

#include <string>
#include <vector>
#include <memory>

enum class AsmIRNodeType { PROGRAM, FUNCTION, MOV, IMMEDIATE, RETURN, REGISTER, INSTRUCTIONS, ALLOCATE_STACK, NEG, NOT, PSEUDO, STACK, UNARY, BINARY, IDIV, CDQ, ADD, SUBTRACT, MULTIPLY };

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

class AsmIRUnary : public AsmIRNode {
public:
    std::unique_ptr<AsmIRNode> unary_operator;
    std::unique_ptr<AsmIRNode> operand;
    AsmIRUnary(std::unique_ptr<AsmIRNode> unary_operator, std::unique_ptr<AsmIRNode> operand);
};

class AsmIRBinary : public AsmIRNode {
public:
    std::unique_ptr<AsmIRNode> binary_operator;
    std::unique_ptr<AsmIRNode> operand1;
    std::unique_ptr<AsmIRNode> operand2;
    AsmIRBinary(std::unique_ptr<AsmIRNode> binary_operator, std::unique_ptr<AsmIRNode> operand1, std::unique_ptr<AsmIRNode> operand2);
};

class AsmIRIdiv : public AsmIRNode {
public:
    std::unique_ptr<AsmIRNode> operand;
    AsmIRIdiv(std::unique_ptr<AsmIRNode> operand);
};

class AsmIRCdq : public AsmIRNode {
public:
    AsmIRCdq();
};

class AsmIRAllocateStack : public AsmIRNode {
public:
    int stack_size;
    AsmIRAllocateStack(int stack_size);
};

class AsmIRNeg : public AsmIRNode {
public:
    AsmIRNeg();
};

class AsmIRNot : public AsmIRNode {
public:
    AsmIRNot();
};

class AsmIRAdd : public AsmIRNode {
public:
    AsmIRAdd();
};

class AsmIRSubtract : public AsmIRNode {
    AsmIRSubtract();
};

class AsmIRMultiply : public AsmIRNode {
    AsmIRMultiply();
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

class AsmIRPseudo : public AsmIRNode {
public:
    std::string identifier;
    AsmIRPseudo(std::string identifier);
};

class AsmIRStack : public AsmIRNode {
public:
    int stack_size;
    AsmIRStack(int stack_size);
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
