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

#include "compile_compile.h"
#include "get_const.h"
#include "../c/tools/getnumberasuint64.h"

void Compile8080::CompileOperator8(CNodePtr &node) {
    assert(node->type == CNT_OPERATOR);
    assert(node->a != nullptr);
    assert(node->b != nullptr);
    assert(node->a->ctype.Is8BitType());

    // Compile: OPERATOR(op, x, NUMBER)
    if (node->b->type == CNT_NUMBER)
        if (CompileOperator8Number(node))
            return;

    // Compile: OPERATOR(op, x, CONST)
    if (node->b->IsConstNode()) {
        switch (node->operator_code) {
            case COP_ADD:
                return CompileOperator8Const(node, ALU_ADD);  // add const
            case COP_SUB:
                return CompileOperator8Const(node, ALU_SUB);  // sub const
            case COP_AND:
                return CompileOperator8Const(node, ALU_AND);  // and const
            case COP_OR:
                return CompileOperator8Const(node, ALU_OR);  // or const
            case COP_XOR:
                return CompileOperator8Const(node, ALU_XOR);  // xor const
        }
    }

    // Compile: OPERATOR(op, x, DEADDR(y))
    if (node->b->IsDeaddr()) {
        switch (node->operator_code) {
            case COP_ADD:
                return CompileOperator8Deaddr(node, ALU_ADD);  // add (hl)
            case COP_SUB:
                return CompileOperator8Deaddr(node, ALU_SUB);  // sub (hl)
            case COP_AND:
                return CompileOperator8Deaddr(node, ALU_AND);  // and (hl)
            case COP_OR:
                return CompileOperator8Deaddr(node, ALU_OR);  // or (hl)
            case COP_XOR:
                return CompileOperator8Deaddr(node, ALU_XOR);  // xor (hl)
        }
    }

    // Compile: OPERATOR(op, x, y)
    CompileAD(node->a, node->b, CanOperatorArgumentsSwapped(node));
    switch (node->operator_code) {
        case COP_ADD:
            return out.add_a_d();
        case COP_SUB:
            return out.sub_a_d();
        case COP_AND:
            return out.and_a_d();
        case COP_OR:
            return out.or_a_d();
        case COP_XOR:
            return out.xor_a_d();
        case COP_MUL:
            return CallInternal(node->ctype.IsUnsigned() ? o.mul_u8 : o.mul_i8);
        case COP_DIV:
            return CallInternal(node->ctype.IsUnsigned() ? o.div_u8 : o.div_i8);
        case COP_MOD:
            return CallInternal(node->ctype.IsUnsigned() ? o.mod_u8 : o.mod_i8);
        case COP_SHL:
            return CallInternal(o.shl_8);
        case COP_SHR:
            return CallInternal(node->ctype.IsUnsigned() ? o.shr_u8 : o.shr_i8);
        default:
            C_INTERNAL_ERROR(node, std::string("unsupported operator ") + ToString(node->operator_code));
    }
}

void Compile8080::CompileOperator8Deaddr(CNodePtr &node, AsmAlu alu) {
    assert(node->type == CNT_OPERATOR);
    assert(node->a != nullptr);
    assert(node->b != nullptr);
    assert(node->a->ctype.Is8BitType());
    assert(node->b->IsDeaddr());

    AsmRegister a_reg = CompileExpression(node->a);
    assert(a_reg == R8_A);
    AsmRegister b_reg = CompileExpression(node->b->a);
    assert(b_reg == R16_HL);
    out.alu_a_reg(alu, R8_M);
}

void Compile8080::CompileOperator8Const(CNodePtr &node, AsmAlu alu) {
    assert(node->type == CNT_OPERATOR);
    assert(node->a != nullptr);
    assert(node->b != nullptr);
    assert(node->a->ctype.Is8BitType());
    assert(node->b->IsConstNode());

    AsmRegister a_reg = CompileExpression(node->a);
    assert(a_reg == R8_A);

    if (node->b->type == CNT_NUMBER)
        out.alu_a_number(alu, GetNumberAsUint64(node->b));
    else
        out.alu_a_string(alu, GetConst(node->b, nullptr, &out));
}

void Compile8080::CompileAD(CNodePtr &a, CNodePtr &b, bool can_swap_result) {
    const bool no_swap_order = a->compiler.hl_contains_value || b->compiler.hl_contains_value;

    if (!no_swap_order && can_swap_result && CheckExpressionOnlyD(a)) {
        AsmRegister b_reg = CompileExpression(b);
        assert(b_reg == R8_A);
        AsmRegister a_reg = CompileExpression(a, CF_ALT);
        assert(a_reg == R8_D);
        return;
    }

    if (CheckExpressionOnlyD(b)) {
        AsmRegister a_reg = CompileExpression(a);
        assert(a_reg == R8_A);
        AsmRegister b_reg = CompileExpression(b, CF_ALT);
        assert(b_reg == R8_D);
        return;
    }

    const uint32_t a_used_regs = CheckExpression(a);
    if (!no_swap_order && (a_used_regs & U_D) == 0) {
        AsmRegister b_reg = CompileExpression(b, CF_CAN_USE_ALT);
        if (b_reg == R8_A) {
            out.ld_d_a();
            b_reg = R8_D;
        }
        assert(b_reg == R8_D);
        AsmRegister a_reg = CompileExpression(a);
        assert(a_reg == R8_A);
        return;
    }

    const uint32_t b_used_regs = CheckExpression(b);
    if (can_swap_result && (b_used_regs & U_D) == 0) {
        AsmRegister a_reg = CompileExpression(a, CF_CAN_USE_ALT);
        if (a_reg == R8_A) {
            out.ld_d_a();
            a_reg = R8_D;
        }
        assert(a_reg == R8_D);
        AsmRegister b_reg = CompileExpression(b);
        assert(b_reg == R8_A);
        return;
    }

    if (!no_swap_order) {
        AsmRegister b_reg = CompileExpression(b, CF_CAN_USE_ALT);
        if (b_reg == R8_A) {
            out.push_af();
        } else {
            assert(b_reg == R8_D);
            out.push_de();
        }
        AsmRegister a_reg = CompileExpression(a);
        assert(a_reg == R8_A);
        out.pop_de();
        b_reg = R8_D;
        return;
    }

    AsmRegister a_reg = CompileExpression(a, CF_CAN_USE_ALT);
    if (a_reg == R8_A) {
        out.push_af();
    } else {
        assert(a_reg == R8_D);
        out.push_de();
    }
    AsmRegister b_reg = CompileExpression(b, CF_CAN_USE_ALT);
    if (b_reg == R8_D) {
        out.pop_af();
        a_reg = R8_A;
    } else {
        assert(b_reg == R8_A);
        if (can_swap_result) {
            out.pop_de();
            a_reg = R8_D;
        } else {
            out.ld_d_a();
            b_reg = R8_D;
            out.pop_af();
            a_reg = R8_A;
        }
    }
}
