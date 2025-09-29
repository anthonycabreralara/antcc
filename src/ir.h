#ifndef IR_H
#define IR_H

#include <string>
#include <vector>
#include <memory>

enum class IRNodeType { PROGRAM, FUNCTION, MOV, IMMEDIATE, RETURN, REGISTER };

class IRNode {
public:
    IRNodeType type;
    virtual ~IRNode() = default;
};

class IRRet : public IRNode {
public:
    IRRet();
};

class IRImm : public IRNode {
public:
    std::string value;
    IRImm (std::string v);
};

class IRMov : public IRNode {
public:
    std::unique_ptr<IRNode> src;
    std::unique_ptr<IRNode> dst;
    IRMov(std::unique_ptr<IRNode> s, std::unique_ptr<IRNode> d);
};

class IRReg : public IRNode {
public:
    std::string value;
    IRReg (std::string v);
};

class IRFunction : public IRNode {
public:
    std::string name;
    std::vector<std::unique_ptr<IRNode>> instructions;
    IRFunction(std::string n, std::vector<std::unique_ptr<IRNode>>&& i);
};

class IRProgram : public IRNode {
public:
    std::unique_ptr<IRFunction> function;
    IRProgram(std::unique_ptr<IRFunction> func);
};

#endif
