#ifndef TACKY_IR_H
#define TACKY_IR_H

#include <string>
#include <vector>
#include <memory>

enum class TackyIRNodeType {PROGRAM, FUNCTION, RETURN, CONSTANT, NEGATE, COMPLEMENT, VAR, UNARY};

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

#endif