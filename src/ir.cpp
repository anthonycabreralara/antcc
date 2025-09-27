#include "ir.h"
#include <cstdio>

// ---------------- Operands -----------------
void IRRegister::emit() const {
    printf("%s", name.c_str());
}

void IRImm::emit() const {
    printf("%d", value);
}

// ---------------- Instructions -------------
IRMov::IRMov(std::unique_ptr<IROperand> d, std::unique_ptr<IROperand> s)
    : dst(std::move(d)), src(std::move(s)) {}

void IRMov::emit() const {
    printf("    mov ");
    dst->emit();
    printf(", ");
    src->emit();
    printf("\n");
}

void IRRet::emit() const {
    printf("    ret\n");
}

// ---------------- Function & Program -------
void IRFunction::emit() const {
    printf("%s:\n", name.c_str());
    for (const auto& instr : instructions) {
        instr->emit();
    }
    printf("\n");
}

void IRProgram::emit() const {
    for (const auto& func : functions) {
        func->emit();
    }
}
