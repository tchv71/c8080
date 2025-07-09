/*
 * c8080 compiler
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "cmm.h"
#include "names.h"
#include <limits.h>
#include "../asm/asm2.h"
#include "../CompileVariable.h"
#include "../cmm/prepare.h"

class Cmm8080 {
private:
    class CmmArg {
    public:
        bool addr{};
        AsmRegister reg{REG_NONE};
        std::string text;
    };

    CProgramm &p;
    Asm2 out;
    AsmLabel *break_label{};
    AsmLabel *continue_label{};

    void CompileDeclareVariable(CNodePtr &i);
    AsmCondition CompileCmmCond(CNode &n);
    AsmRegister CompileRegister(CNodePtr &n);
    void CompileCmmArg(CNodePtr &n, CmmArg &arg);
    void CompileLine(CNodePtr &n, CmmArg &arg);
    void CompileLevel(CNodePtr &s);
    void CompileSpecialFunctions(CNodePtr &n, CmmArg &arg);
    void CompileRotateFunctions(CNodePtr &n, AssemblerCommand opcode);
    bool CompileOptionalConstArg(CNodePtr &arg, uint64_t &outResult);
    uint64_t CompileOptionalConstArg(CNodePtr &n, uint64_t defultValue, uint64_t maxValue);
    bool IsDoNothingArgs(CNodePtr &n);
    void CompileCmm(CNodePtr &n);

public:
    Cmm8080(CProgramm &p_) : p(p_) {
    }
    void Compile(CString asm_file_name);
};

void Cmm8080::CompileLevel(CNodePtr &s) {
    for (CNodePtr i = s; i; i = i->next_node) {
        CmmArg arg;
        CompileLine(i, arg);
    }
}

void Cmm8080::CompileDeclareVariable(CNodePtr &i) {
    CVariable &v = *i->variable;

    assert(!v.output_name.empty());

    if (i->extern_flag || (v.type.pointers.empty() && v.type.flag_const)) {
        if (v.address_attribute.exists) {
            out.source(i->e);
            out.equ(v.output_name, std::to_string(v.address_attribute.value));
        }
        return;
    }

    // The last command of the function calls the next function
    if (!out.lines_.empty()) {
        auto &b = out.lines_.back();
        if (b.opcode == AC_JMP && b.argument[0].string == v.output_name)
            out.lines_.pop_back();
    }

    out.source(i->e);

    if (!v.type.IsFunction()) {
        out.MakeFile();
        CompileVariable(out, p, i->variable);
        return;
    }

    PrepareCmm(out, p, i);

    out.label(v.output_name);

    CompileLevel(v.body->a);

    // The last command of the function calls function
    bool need_ret = true;
    if (!out.lines_.empty()) {
        auto &b = out.lines_.back();
        switch (b.opcode) {
            case AC_CALL:
                b.opcode = AC_JMP;
            case AC_PCHL:
            case AC_JMP:
            case AC_RET:
                need_ret = false;
                break;
        }
    }

    if (need_ret)
        out.ret();
}

void Cmm8080::Compile(CString asm_file_name) {
    out.buffer = "    device zxspectrum48 ; There is no ZX Spectrum, it is needed for the sjasmplus assembler.\n";

    for (CNodePtr i = p.first_node; i; i = i->next_node) {
        switch (i->type) {
            case CNT_TYPEDEF:
                break;
            case CNT_ASM:
                out.assembler(i->text);
                break;
            case CNT_DECLARE_VARIABLE:
                CompileDeclareVariable(i);
                break;
            default:
                p.Error(i->e, "Unsupported node " + ToString(i->type));
        }
    }

    out.MakeFile();

    FsTools::SaveFile(asm_file_name, out.buffer);
}

void CompileCmm8080(CProgramm &p, CString asm_file_name) {
    Cmm8080 c(p);
    c.Compile(asm_file_name);
}
