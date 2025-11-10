#ifndef TACKY_IR_H
#define TACKY_IR_H

#include <string>
#include <vector>
#include <memory>

enum class TackyIRNodeType {
    PROGRAM, 
    FUNCTION, 
    RETURN, 
    CONSTANT, 
    COMPLEMENT,
    NEGATE,
    NOT,
    VAR, 
    UNARY, 
    BINARY, 
    COPY,
    JUMP,
    JUMP_IF_ZERO,
    JUMP_IF_NOT_ZERO,
    LABEL, 
    ADD, 
    SUBTRACT, 
    MULTIPLY, 
    DIVIDE, 
    REMAINDER,
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_OR_EQUAL,
    GREATER_THAN,
    GREATER_OR_EQUAL
};

class TackyIRNode {
public:
    TackyIRNodeType type;
    virtual ~TackyIRNode() = default;
};

class TackyIRProgram : public TackyIRNode {
public:
    std::unique_ptr<TackyIRNode> function;
    TackyIRProgram(std::unique_ptr<TackyIRNode> func);
};

class TackyIRInstructions : public TackyIRNode {
public:
    std::vector<std::unique_ptr<TackyIRNode>> instructions;

    TackyIRInstructions();
};

class TackyIRFunction : public TackyIRNode {
public:
    std::string name;
    std::unique_ptr<TackyIRInstructions> instructions;

    TackyIRFunction(std::string n, std::unique_ptr<TackyIRInstructions> instr);
};

class TackyIRReturn : public TackyIRNode {
public:
    std::unique_ptr<TackyIRNode> expr;
    TackyIRReturn(std::unique_ptr<TackyIRNode> e);
};

class TackyIRComplement : public TackyIRNode {
public:
    TackyIRComplement();
};

class TackyIRNegate : public TackyIRNode {
public:
    TackyIRNegate();
};

class TackyIRNot : public TackyIRNode {
public:
    TackyIRNot();
};

class TackyIRAdd : public TackyIRNode {
public:
    TackyIRAdd();
};

class TackyIRSubtract : public TackyIRNode {
public:
    TackyIRSubtract();
};

class TackyIRMultiply : public TackyIRNode {
public:
    TackyIRMultiply();
};

class TackyIRDivide : public TackyIRNode {
public:
    TackyIRDivide();
};

class TackyIRRemainder : public TackyIRNode {
public:
    TackyIRRemainder();
};

class TackyIREqual : public TackyIRNode {
public:
    TackyIREqual();
};

class TackyIRNotEqual : public TackyIRNode {
public:
    TackyIRNotEqual();
};

class TackyIRLessThan : public TackyIRNode {
public:
    TackyIRLessThan();
};

class TackyIRLessOrEqual : public TackyIRNode {
public:
    TackyIRLessOrEqual();
};

class TackyIRGreaterThan : public TackyIRNode {
public:
    TackyIRGreaterThan();
};

class TackyIRGreaterOrEqual : public TackyIRNode {
public:
    TackyIRGreaterOrEqual();
};

class TackyIRConstant : public TackyIRNode {
public:
    std::string value;
    TackyIRConstant(std::string v);
};

class TackyIRVar : public TackyIRNode {
public:
    std::string value;
    TackyIRVar(std::string v);
};

class TackyIRUnary : public TackyIRNode {
public:
    std::unique_ptr<TackyIRNode> op;
    std::unique_ptr<TackyIRNode> src;
    std::unique_ptr<TackyIRNode> dst;
    TackyIRUnary(std::unique_ptr<TackyIRNode> o, std::unique_ptr<TackyIRNode> s, std::unique_ptr<TackyIRNode> d);
};

class TackyIRBinary : public TackyIRNode {
public:
    std::unique_ptr<TackyIRNode> op;
    std::unique_ptr<TackyIRNode> src1;
    std::unique_ptr<TackyIRNode> src2;
    std::unique_ptr<TackyIRNode> dst;
    TackyIRBinary(std::unique_ptr<TackyIRNode> op, std::unique_ptr<TackyIRNode> src1, std::unique_ptr<TackyIRNode> src2, std::unique_ptr<TackyIRNode> dst);
};

class TackyIRCopy : public TackyIRNode {
public:
    std::unique_ptr<TackyIRNode> src;
    std::unique_ptr<TackyIRNode> dst;
    TackyIRCopy(std::unique_ptr<TackyIRNode> src, std::unique_ptr<TackyIRNode> dst);
};

class TackyIRJump : public TackyIRNode {
public:
    std::string target;
    TackyIRJump(std::string target);
};

class TackyIRJumpIfZero : public TackyIRNode {
public:
    std::unique_ptr<TackyIRNode> target;
    std::string condition;
    TackyIRJumpIfZero(std::unique_ptr<TackyIRNode> target, std::string condition);
};

class TackyIRJumpIfNotZero : public TackyIRNode {
public:
    std::unique_ptr<TackyIRNode> condition;
    std::unique_ptr<TackyIRNode> target;
    TackyIRJumpIfNotZero(std::unique_ptr<TackyIRNode> target, std::unique_ptr<TackyIRNode> condition);
};

class TackyIRLabel : public TackyIRNode {
public:
    std::string identifier;
    TackyIRLabel(std::string identifier);
};



#endif