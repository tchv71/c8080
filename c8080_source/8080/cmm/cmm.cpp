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
#include "../../c/tools/cthrow.h"
#include "../../tools/fs_tools.h"

class Cmm8080 {
private:
    class Arg {
    public:
        AsmRegister reg{REG_NONE};
        bool addr{};
        std::string text;

        bool IsReg(AsmRegister r) {
            return !addr && reg == r;
        }

        bool IsA() {
            return IsReg(R8_A);
        }

        bool IsHl() {
            return IsReg(R16_HL);
        }

        bool Is16Sp() {
            return !addr && IsAsmRegister16Sp(reg);
        }

        bool Is16Af() {
            return !addr && IsAsmRegister16Af(reg);
        }

        bool IsConst() {
            return !addr && !text.empty();
        }

        bool IsConstAddr() {
            return addr && !text.empty();
        }
    };

    CProgramm &p;
    Asm2 out;
    AsmLabel *break_label{};
    AsmLabel *continue_label{};

    void CompileDeclareVariable(CNodePtr &i);
    AsmCondition CompileCond(CNode &n);
    AsmRegister CompileRegister(CNodePtr &n);
    void CompileArg(CNodePtr &n, Arg &arg);
    void CompileLine(CNodePtr &n, Arg &arg);
    void CompileLevel(CNodePtr &n);
    void CompileSpecialFunctions(CNodePtr &n, Arg &arg);
    void CompileRotate(CNodePtr &n, AssemblerCommand opcode);
    void CompileAlu(CNodePtr &n, AsmAlu alu);
    bool IsDoNothingArgs(CNodePtr &n);
    void CompileCmm(CNodePtr &n);
    AsmCondition CmmNameToAsmCondition(CNode &v);
    void CompileArgs_Empty(CNodePtr &n);
    void CompileArgs_A(CNodePtr &n);
    uint64_t CompileArgs_A_OptionalNumber(CNodePtr &n, uint64_t value, uint64_t max_value);
    void CompileArgs_A_R8_M_Number(CNodePtr &n, Arg &arg);

public:
    Cmm8080(CProgramm &p_) : p(p_) {
    }
    void Compile(CString asm_file_name);
};

void Cmm8080::CompileArg(CNodePtr &n, Arg &arg) {
    while (n->type == CNT_CONVERT)  // TODO: Remove
        n = n->a;
    bool addr = n->type == CNT_MONO_OPERATOR && n->mono_operator_code == MOP_DEADDR;
    CompileLine(addr ? n->a : n, arg);
    arg.addr = addr;
    if (arg.addr && arg.reg == R16_HL) {
        arg.addr = false;
        arg.reg = R8_M;
    }
}

AsmCondition Cmm8080::CmmNameToAsmCondition(CNode &n) {
    switch (n.variable->c.internal_cmm_name) {
        case CMM_NAME_FLAG_Z:
            return JC_Z;
        case CMM_NAME_FLAG_NZ:
            return JC_NZ;
        case CMM_NAME_FLAG_C:
            return JC_C;
        case CMM_NAME_FLAG_NC:
            return JC_NC;
        case CMM_NAME_FLAG_M:
            return JC_M;
        case CMM_NAME_FLAG_P:
            return JC_P;
        case CMM_NAME_FLAG_PE:
            return JC_PE;
        case CMM_NAME_FLAG_PO:
            return JC_PO;
    }
    p.Error(n.e, "only flag_");
    return JC_Z;
}

AsmCondition Cmm8080::CompileCond(CNode &n) {
    switch (n.type) {
        case CNT_CONVERT:
            return CompileCond(*n.a);  // TODO: Remove
        case CNT_OPERATOR: {
            if (!n.a || !n.b)
                C_ERROR_INTERNAL(n.e, "only two arguments");

            Arg a, b;
            CompileArg(n.a, a);
            CompileArg(n.b, b);

            if (!a.IsA())
                p.Error(n.a->e, "only a");

            if (b.text == "0" && (n.operator_code == COP_CMP_E || n.operator_code == COP_CMP_NE))
                out.alu_a_reg(ALU_OR, R8_A);
            else if (!b.addr && IsAsmRegister8M(b.reg))
                out.alu_a_reg(ALU_CMP, b.reg);
            else if (!b.addr && !b.text.empty())
                out.alu_a_string(ALU_CMP, b.text);
            else
                p.Error(n.b->e, "only 8 bit register, *hl or const");

            switch (n.operator_code) {
                case COP_CMP_L:
                    return JC_C;
                case COP_CMP_GE:
                    return JC_NC;
                case COP_CMP_E:
                    return JC_Z;
                case COP_CMP_NE:
                    return JC_NZ;
            }

            p.Error(n.e, "only < >= == !=");
            return JC_Z;
        }
        case CNT_MONO_OPERATOR:
            // Example: if (flag_z) ...
            if (n.mono_operator_code == MOP_ADDR && n.a->type == CNT_LOAD_VARIABLE)
                return CmmNameToAsmCondition(*n.a);
            p.Error(n.e, std::string("unsupported mono operator ") + ToString(n.mono_operator_code));
            return JC_Z;
        case CNT_FUNCTION_CALL:
            // Example: if (flag_z(...)) ...
            CompileLevel(n.a);
            return CmmNameToAsmCondition(n);
    }
    p.Error(n.e, "unsupported node " + ToString(n.type));
    return JC_Z;
}

AsmRegister Cmm8080::CompileRegister(CNodePtr &n) {
    switch (n->variable->c.internal_cmm_name) {
        case CMM_NAME_A:
            return R8_A;
        case CMM_NAME_B:
            return R8_B;
        case CMM_NAME_C:
            return R8_C;
        case CMM_NAME_D:
            return R8_D;
        case CMM_NAME_E:
            return R8_E;
        case CMM_NAME_H:
            return R8_H;
        case CMM_NAME_L:
            return R8_L;
        case CMM_NAME_BC:
            return R16_BC;
        case CMM_NAME_DE:
            return R16_DE;
        case CMM_NAME_HL:
            return R16_HL;
        case CMM_NAME_SP:
            return R16_SP;
    }
    p.Error(n->e, "only register");
    return R8_A;
}

void Cmm8080::CompileArgs_Empty(CNodePtr &n) {
    if (n->a)
        p.Error(n->e, "only ()");
}

void Cmm8080::CompileArgs_A(CNodePtr &n) {
    if (n->a) {
        Arg a;
        CompileArg(n->a, a);
        if (a.IsA() && !n->a->next_node)
            return;
    }
    p.Error(n->e, "only (a)");
}

uint64_t Cmm8080::CompileArgs_A_OptionalNumber(CNodePtr &n, uint64_t default_value, uint64_t max_value) {
    if (!n->a) {
        p.Error(n->e, "only (a) or (a, const)");
        return default_value;
    }

    Arg a;
    CompileArg(n->a, a);
    if (!a.IsA()) {
        p.Error(n->e, "only (a) or (a, const)");
        return default_value;
    }

    if (!n->a->next_node)
        return default_value;

    Arg b;
    CompileArg(n->a->next_node, b);
    if (b.text.empty()) {
        p.Error(n->e, "only (a) or (a, const)");
        return default_value;
    }

    char *end = nullptr;
    errno = 0;
    const uint64_t value = strtoul(b.text.c_str(), &end, 0);
    if (end[0] != 0 || errno != 0) {
        p.Error(n->e, "only (a) or (a, const)");
        return default_value;
    }

    if (value > max_value) {
        p.Error(n->e, "maximal value is " + std::to_string(max_value));
        return default_value;
    }

    if (n->a->next_node->next_node)
        p.Error(n->e, "only (a) or (a, const)");

    return value;
}

void Cmm8080::CompileArgs_A_R8_M_Number(CNodePtr &n, Arg &arg) {
    if (n->a && n->a->next_node && !n->a->next_node->next_node) {
        Arg a;
        CompileArg(n->a, a);
        if (a.IsA()) {
            Arg b;
            CompileArg(n->a->next_node, arg);
            if (!arg.addr && IsAsmRegister8M(arg.reg))
                return;
            if (!arg.addr && !arg.text.empty())
                return;
        }
    }
    arg = Arg{R8_A};
    p.Error(n->e, "only (a, r8) or (a, *hl) or (a, const)");
}

void Cmm8080::CompileRotate(CNodePtr &n, AssemblerCommand opcode) {
    uint64_t count = CompileArgs_A_OptionalNumber(n, 1, 8);
    for (uint64_t i = 0; i < count; i++)
        out.rotate(opcode);
}

void Cmm8080::CompileAlu(CNodePtr &n, AsmAlu alu) {
    Arg a;
    CompileArgs_A_R8_M_Number(n, a);
    if (a.text.empty())
        out.alu_a_reg(alu, a.reg);
    else
        out.alu_a_string(alu, a.text);
}

void Cmm8080::CompileSpecialFunctions(CNodePtr &n, Arg &arg) {
    switch (n->variable->c.internal_cmm_name) {
        case CMM_NAME_SET_FLAG_C:
            CompileArgs_Empty(n);
            out.stc();
            return;
        case CMM_NAME_INVERT:
            CompileArgs_A(n);
            out.cpl();
            return;
        case CMM_NAME_ENABLE_INTERRUPTS:
            CompileArgs_Empty(n);
            out.ei();
            return;
        case CMM_NAME_DISABLE_INTERRUPTS:
            CompileArgs_Empty(n);
            out.di();
            return;
        case CMM_NAME_NOP:
            CompileArgs_Empty(n);
            out.nop();
            return;
        case CMM_NAME_DAA:
            CompileArgs_Empty(n);
            out.daa();
            return;
        case CMM_NAME_CYCLIC_ROTATE_LEFT:
            return CompileRotate(n, AC_RLCA);
        case CMM_NAME_CYCLIC_ROTATE_RIGHT:
            return CompileRotate(n, AC_RRCA);
        case CMM_NAME_CARRY_ROTATE_LEFT:
            return CompileRotate(n, AC_RLA);
        case CMM_NAME_CARRY_ROTATE_RIGHT:
            return CompileRotate(n, AC_RRA);
        case CMM_NAME_COMPARE:
            return CompileAlu(n, ALU_CMP);
        case CMM_NAME_CARRY_ADD:
            return CompileAlu(n, ALU_ADC);
        case CMM_NAME_CARRY_SUB:
            return CompileAlu(n, ALU_SBC);
        case CMM_NAME_PUSH:
            for (auto i = n->a; i; i = i->next_node) {
                Arg a;
                CompileArg(i, a);
                if (a.reg == R8_A)
                    a.reg = R16_AF;
                if (!a.Is16Af()) {
                    p.Error(n->e, "only a, bc, de, hl");
                    a.reg = R16_HL;
                }
                out.push_reg(a.reg);
            }
            break;
        case CMM_NAME_POP: {
            if (!IsDoNothingArgs(n->a))
                p.Error(n->e, "only registers in arguments");
            std::vector<CNodePtr> items;
            for (auto i = n->a; i; i = i->next_node)
                items.push_back(i);
            for (auto i = items.rbegin(); i != items.rend(); i++) {
                Arg a;
                CompileArg(*i, a);
                if (a.reg == R8_A)
                    a.reg = R16_AF;
                if (!a.Is16Af()) {
                    p.Error(n->e, "only a, bc, de, hl");
                    a.reg = R16_HL;
                }
                out.pop_reg(a.reg);
            }
            break;
        }
        case CMM_NAME_IN:
            if (!n->a || n->a->next_node)
                p.Error(n->e, "only (const)");
            arg.reg = R32_DEHL;
            arg.text = out.GetConst(n->a);
            break;
        case CMM_NAME_OUT:
            if (n->a && n->a->next_node && !n->a->next_node->next_node) {
                Arg a, b;
                CompileArg(n->a, a);
                CompileArg(n->a->next_node, b);
                if (a.IsConst() && b.IsA()) {
                    out.out(a.text);
                    break;
                }
            }
            p.Error(n->e, "only (const, a)");
            break;
        case CMM_NAME_SWAP:
            if (n->a && n->a->next_node && !n->a->next_node->next_node) {
                Arg a, b;
                CompileArg(n->a, a);
                CompileArg(n->a->next_node, b);

                if (a.reg > b.reg)
                    std::swap(a, b);

                if (!a.addr && !b.addr && a.reg == R16_DE && b.reg == R16_HL) {
                    out.ex_hl_de();
                    return;
                }

                if (!a.addr && b.addr && a.reg == R16_HL && b.reg == R16_SP) {
                    out.ex_psp_hl();
                    return;
                }
            }
            p.Error(n->e, "only (hl, de) or (*sp, hl)");
            break;
        default:
            p.Error(n->e, "syntax error");
    }
}

bool Cmm8080::IsDoNothingArgs(CNodePtr &n) {
    for (CNodePtr i = n; i; i = i->next_node)
        if (i->type != CNT_CONST && i->type != CNT_NUMBER && i->type != CNT_LOAD_VARIABLE)
            return false;
    return true;
}

void Cmm8080::CompileLine(CNodePtr &n, Arg &arg) {
    out.source(n->e);
    switch (n->type) {
        case CNT_LOAD_VARIABLE:
            arg.reg = CompileRegister(n);
            return;
        case CNT_LEVEL:
            CompileLevel(n->a);
            return;
        case CNT_CONVERT:
            CompileLine(n->a, arg);
            return;
        case CNT_CONST:
            assert(!n->text.empty());
            arg.text = n->text;
            return;
        case CNT_NUMBER:
            arg.text = out.GetConst(n);
            return;
        case CNT_FUNCTION_CALL:
            if (n->variable->c.internal_cmm_name != 0) {
                CompileSpecialFunctions(n, arg);
                return;
            }
            CompileLevel(n->a);
            out.call(n->variable->output_name);
            break;
        case CNT_FUNCTION_CALL_ADDR: {
            CompileLevel(n->a);
            Arg a;
            CompileArg(n->b, a);
            if (a.text.empty())
                p.Error(n->b->e, "only const");
            out.call(a.text);
            break;
        }
        case CNT_RETURN: {
            if (n->a) {
                CNodePtr i = n->a;
                while (i->type == CNT_CONVERT)  // TODO: Remove
                    i = i->a;
                if (i->type == CNT_FUNCTION_CALL && i->variable && i->variable->c.internal_cmm_name == CMM_NAME_HL) {
                    if (i->a)
                        p.Error(n->e, "only return hl()");
                    out.pchl();
                    return;
                }
                Arg arg2;
                CompileLine(n->a, arg2);
            }
            bool need_ret = true;
            if (!out.lines.empty()) {
                auto &b = out.lines.back();
                if (b.opcode == AC_CALL) {
                    b.opcode = AC_JMP;
                    need_ret = false;
                } else if (b.opcode == AC_CALL_CONDITION) {
                    b.opcode = AC_JMP_CONDITION;
                    need_ret = false;
                }
            }
            if (need_ret)
                out.ret();
            return;
        }
        case CNT_BREAK:
            if (!break_label)
                p.Error(n->e, "break without loop");
            else
                out.jmp_label(break_label);
            break;
        case CNT_CONTINUE:
            if (!continue_label)
                p.Error(n->e, "continue without loop");
            else
                out.jmp_label(continue_label);
            break;
        case CNT_FOR: {
            CompileLevel(n->a);
            const auto continue_label_ = continue_label;
            const auto break_label_ = break_label;
            const auto for_label = out.AllocLabel();
            continue_label = out.AllocLabel();
            break_label = out.AllocLabel();
            out.label(n->c ? for_label : continue_label);
            if (n->b)
                CompileCond(*n->b);  // TODO
            CompileLevel(n->d);
            if (n->c) {
                out.label(continue_label);
                CompileLevel(n->c);
            }
            out.jmp_label(n->c ? for_label : continue_label);
            if (break_label->used != 0)
                out.label(break_label);
            break_label = break_label_;
            continue_label = continue_label_;
            break;
        }
        case CNT_DO: {
            const auto loop_label = out.AllocLabel();
            const auto prev_continue_label = continue_label;
            const auto prev_break_label = break_label;
            continue_label = out.AllocLabel();
            break_label = out.AllocLabel();
            out.label(n->a ? loop_label : continue_label);
            CompileLevel(n->b);
            if (n->a) {
                out.label(continue_label);
                AsmCondition cond = CompileCond(*n->a);
                out.jmp_condition_label(cond, loop_label);
            } else {
                out.jmp_label(n->a ? loop_label : continue_label);
            }
            if (break_label->used != 0)
                out.label(break_label);
            break_label = prev_break_label;
            continue_label = prev_continue_label;
            break;
        }
        case CNT_IF: {
            AsmCondition cond = CompileCond(*n->a);

            if (n->b && !n->b->next_node && !n->c) {
                out.source(n->b->e);

                if (n->b->type == CNT_RETURN) {
                    if (!n->b->a) {
                        out.ret_condition(cond);
                        return;
                    }
                    if (n->b->a->type == CNT_FUNCTION_CALL && n->b->a->variable->c.internal_cmm_name == 0 &&
                        IsDoNothingArgs(n->b->a->a) && !n->b->a->next_node) {
                        out.jmp_condition(cond, n->b->a->variable->output_name);
                        return;
                    }
                }
                if (n->b->type == CNT_FUNCTION_CALL && n->b->variable->c.internal_cmm_name == 0 &&
                    IsDoNothingArgs(n->b->a)) {
                    out.call2_condition(cond, n->b->variable->output_name);
                    return;
                }
                if (n->b->type == CNT_GOTO) {
                    out.jmp_condition(cond, n->b->variable->output_name);
                    return;
                }
                if (n->b->type == CNT_BREAK) {
                    if (!break_label)
                        p.Error(n->e, "break without loop");
                    out.jmp_condition_label(cond, break_label);
                    return;
                }
                if (n->b->type == CNT_CONTINUE) {
                    if (!continue_label)
                        p.Error(n->e, "continue without loop");
                    out.jmp_condition_label(cond, continue_label);
                    return;
                }
            }

            if (!InvertAsmJumpCondition(cond))
                throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " InvertJumpCondition " +
                                         std::to_string(cond));

            const auto else_label = out.AllocLabel();
            const auto endif_label = out.AllocLabel();
            if (n->a)
                out.jmp_condition_label(cond, else_label);
            else
                out.jmp_label(else_label);

            CompileLevel(n->b);

            if (n->c)
                out.jmp_label(endif_label);
            out.label(else_label);
            if (n->c) {
                CompileLevel(n->c);
                out.label(endif_label);
            }
            break;
        }
        case CNT_PUSH_POP: {
            if (!IsDoNothingArgs(n->a))
                p.Error(n->e, "only registers in arguments");

            std::vector<AsmRegister> items;
            for (auto i = n->a; i; i = i->next_node) {
                Arg a;
                CompileArg(i, a);
                if (a.reg == R8_A)
                    a.reg = R16_AF;
                if (a.addr || !IsAsmRegister16Af(a.reg))
                    p.Error(n->e, "only a, bc, de, hl");
                items.push_back(a.reg);
                out.push_reg(a.reg);
            }
            CompileLevel(n->b);
            for (auto i = items.rbegin(); i != items.rend(); i++)
                out.pop_reg(*i);
            break;
        }
        case CNT_OPERATOR: {
            Arg b;
            CompileArg(n->a, arg);
            CompileArg(n->b, b);

            if (n->operator_code == COP_SET_ADD && arg.IsHl() && b.Is16Sp()) {
                out.add_hl_reg(b.reg);
                return;
            }

            if (n->operator_code == COP_SET) {
                if (!b.addr && b.reg == R32_DEHL) {
                    if (!arg.IsA())
                        p.Error(n->e, "only a = in(number)");
                    out.in(b.text);
                    return;
                }
                if (!arg.addr && IsAsmRegister8M(arg.reg) && !b.addr && IsAsmRegister8M(b.reg)) {
                    if (arg.reg == R8_M && b.reg == R8_M) {
                        p.Error(n->e, "*hl = *hl is prohibited");
                        arg.reg = R8_A;
                    }
                    out.ld_r8_r8(arg.reg, b.reg);
                    return;
                }
                if (!arg.addr && IsAsmRegister8M(arg.reg) && b.IsConst()) {
                    out.ld_r8_string(arg.reg, b.text);
                    return;
                }
                if (arg.Is16Sp() && b.IsConst()) {
                    out.ld_r16_string(arg.reg, b.text);
                    return;
                }
                if (arg.IsConstAddr() && b.IsA()) {
                    out.ld_pstring1_a(arg.text);
                    return;
                }
                if (arg.IsConstAddr() && b.IsHl()) {
                    out.ld_pstring_hl(arg.text);
                    return;
                }
                if (arg.IsA() && b.IsConstAddr()) {
                    out.ld_a_pstring(b.text);
                    return;
                }
                if (arg.IsHl() && b.IsConstAddr()) {
                    out.ld_hl_pstring(b.text);
                    return;
                }
                if (arg.IsReg(R16_SP) && b.IsHl()) {
                    out.ld_sp_hl();
                    return;
                }
                if (arg.IsA() && b.addr && (b.reg == R16_BC || b.reg == R16_DE)) {
                    out.ld_a_preg(b.reg);
                    return;
                }
                if (arg.addr && (arg.reg == R16_BC || arg.reg == R16_DE) && b.IsA()) {
                    out.ld_preg_a(arg.reg);
                    return;
                }
                p.Error(n->e, "operation not supported");
                return;
            }

            if (arg.reg != R8_A)
                p.Error(n->e, "only A or HL");

            enum AsmAlu alu = ALU_XOR;
            switch (n->operator_code) {
                case COP_SET_ADD:
                    alu = ALU_ADD;
                    break;
                case COP_SET_AND:
                    alu = ALU_AND;
                    break;
                case COP_SET_OR:
                    alu = ALU_OR;
                    break;
                case COP_SET_XOR:
                    alu = ALU_XOR;
                    break;
                case COP_SET_SUB:
                    alu = ALU_SUB;
                    break;
                default:
                    arg.text = out.GetConst(n);
                    return;
            }

            if (IsAsmRegister8M(b.reg))
                out.alu_a_reg(alu, b.reg);
            else if (!b.text.empty())
                out.alu_a_string(alu, b.text);
            else
                p.Error(n->e, "Incorrect argument");

            return;
        }
        case CNT_MONO_OPERATOR:
            switch (n->mono_operator_code) {
                case MOP_POST_INC:
                    CompileArg(n->a, arg);
                    if (!arg.Is16Sp() && !IsAsmRegister8M(arg.reg))
                        p.Error(n->e, "Incorrect argument");
                    out.inc_reg(arg.reg);
                    return;
                case MOP_POST_DEC:
                    CompileArg(n->a, arg);
                    if (!arg.Is16Sp() && !IsAsmRegister8M(arg.reg))
                        p.Error(n->e, "Incorrect argument");
                    out.dec_reg(arg.reg);
                    return;
                case MOP_ADDR:
                    if (n->a->type == CNT_LOAD_VARIABLE) {
                        arg.reg = CompileRegister(n->a);
                        return;
                    }
                    arg.text = out.GetConst(n->a);
                    return;
                default:
                    p.Error(n->e, "Unsupported monooperator " + std::to_string(n->operator_code));
            }
            break;
        case CNT_LABEL:
            if (n->variable->label_call_count > 0)  // No goto
                out.label(n->variable->output_name);
            break;
        case CNT_GOTO:
            out.jmp(n->variable->output_name);
            break;
        case CNT_ASM:
            out.assembler(n->text.c_str());
            break;
        case CNT_DECLARE_VARIABLE:
            out.MakeFile();
            CompileVariable(out, p, n->variable);
            break;
        default:
            p.Error(n->e, "Unsupported node " + ToString(n->type));
    }
}

void Cmm8080::CompileLevel(CNodePtr &n) {
    for (CNodePtr i = n; i; i = i->next_node) {
        Arg arg;
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
    if (!out.lines.empty()) {
        auto &b = out.lines.back();
        if (b.opcode == AC_JMP && b.argument[0].string == v.output_name)
            out.lines.pop_back();
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
    if (!out.lines.empty()) {
        auto &b = out.lines.back();
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
    out.InitBuffer();

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
                p.Error(i->e, "unsupported node " + ToString(i->type));
        }
    }

    out.MakeFile();

    FsTools::SaveFile(asm_file_name, out.buffer);
}

void CompileCmm8080(CProgramm &p, CString asm_file_name) {
    Cmm8080 c(p);
    c.Compile(asm_file_name);
}
