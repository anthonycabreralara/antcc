#ifndef IR_H
#define IR_H

#include <string>
#include <vector>
#include <memory>

// ---------------- Base Node ----------------
struct IRNode {
    virtual ~IRNode() = default;
    virtual void emit() const = 0;  // For final assembly output
};

// ---------------- Operands -----------------
struct IROperand : IRNode {};

struct IRRegister : IROperand {
    std::string name;
    explicit IRRegister(const std::string& n) : name(n) {}
    void emit() const override;
};

struct IRImm : IROperand {
    int value;
    explicit IRImm(int v) : value(v) {}
    void emit() const override;
};

// ---------------- Instructions -------------
struct IRInstr : IRNode {};

struct IRMov : IRInstr {
    std::unique_ptr<IROperand> dst;
    std::unique_ptr<IROperand> src;
    IRMov(std::unique_ptr<IROperand> d, std::unique_ptr<IROperand> s);
    void emit() const override;
};

struct IRRet : IRInstr {
    void emit() const override;
};

// ---------------- Function & Program -------
struct IRFunction : IRNode {
    std::string name;
    std::vector<std::unique_ptr<IRInstr>> instructions;

    explicit IRFunction(const std::string& n) : name(n) {}
    void emit() const override;
};

struct IRProgram : IRNode {
    std::vector<std::unique_ptr<IRFunction>> functions;
    void emit() const override;
};

#endif // IR_H
