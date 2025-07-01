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

#include "Compiler.h"

static const enum AsmAlu ALU_NONE = ALU_SBC;

void Compiler8080::BuildOperator8(CNodePtr &node) {
    node->bi.alu = OperatorCodeToAlu(node->operator_code);
    node->bi.rearrange = IsArgumentsRearrangeAllowed(node->operator_code);

    // First priority
    Measure(node, R8_A, &Compiler8080::Case_Shl8_MC);
    Measure(node, R8_A, &Compiler8080::Case_Shr8_MC);
    Measure(node, R8_A, &Compiler8080::Case_Mul8_MC);
    Measure(node, R8_A, &Compiler8080::Case_Mul8_MCR);
    Measure(node, R8_A, &Compiler8080::Case_Mul8_AC);
    Measure(node, R8_A, &Compiler8080::Case_Mul8_ACR);

    // Last priority
    if (!node->bi.main.able) {
        Measure(node, R8_A, &Compiler8080::Case_Operator8);
        Measure(node, R8_A, &Compiler8080::Case_Operator8_PMR);
        Measure(node, R8_A, &Compiler8080::Case_Operator8_PM);
        Measure(node, R8_A, &Compiler8080::Case_Operator8_MCR);
        Measure(node, R8_A, &Compiler8080::Case_Operator8_MC);
    }
}

AsmAlu Compiler8080::OperatorCodeToAlu(COperatorCode code) {
    switch (code) {
        case COP_CMP_L:
        case COP_CMP_G:
        case COP_CMP_LE:
        case COP_CMP_GE:
        case COP_CMP_E:
        case COP_CMP_NE:
            return ALU_CMP;
        case COP_ADD:
            return ALU_ADD;
        case COP_SUB:
            return ALU_SUB;
        case COP_AND:
            return ALU_AND;
        case COP_OR:
            return ALU_OR;
        case COP_XOR:
            return ALU_XOR;
        default:
            return ALU_NONE;
    }
}

void Compiler8080::Alu8D(CNodePtr &node) {
    assert(node->type == CNT_OPERATOR);
    assert(node->a->ctype.Is8BitType());

    switch (node->operator_code) {
        case COP_CMP_L:
        case COP_CMP_G:
        case COP_CMP_LE:
        case COP_CMP_GE:
        case COP_CMP_E:
        case COP_CMP_NE:
        case COP_ADD:
        case COP_SUB:
        case COP_AND:
        case COP_OR:
        case COP_XOR:
            return out.alu_a_reg(node->bi.alu, R8_D);
        case COP_MUL:
            return InternalCall(node->ctype.IsUnsigned() ? o.mul_u8 : o.mul_i8);
        case COP_DIV:
            return InternalCall(node->ctype.IsUnsigned() ? o.div_u8 : o.div_i8);
        case COP_MOD:
            return InternalCall(node->ctype.IsUnsigned() ? o.mod_u8 : o.mod_i8);
        case COP_SHL:
            return InternalCall(o.shl_8);
        case COP_SHR:
            return InternalCall(node->ctype.IsUnsigned() ? o.shr_u8 : o.shr_i8);
        default:
            C_ERROR_UNSUPPORTED_OPERATOR(node);
    }
}

bool Compiler8080::Case_Operator8(CNodePtr &node, AsmRegister reg) {
    BuildArgs2(node, reg, node->a, node->b, R8_A, R8_D, node->bi.rearrange);
    Alu8D(node);
    return true;
}

bool Compiler8080::Case_Operator8_PM(CNodePtr &node, AsmRegister reg) {
    if (node->bi.alu != ALU_NONE && node->b->IsDeaddr()) {
        if (BuildDouble(node, reg, node->b->a, R16_HL, node->a, R8_A)) {
            out.alu_a_reg(node->bi.alu, R8_M);
            return true;
        }
    }
    return false;
}

bool Compiler8080::Case_Operator8_PMR(CNodePtr &node, AsmRegister reg) {
    if (node->bi.alu != ALU_NONE && node->a->IsDeaddr() && node->bi.rearrange) {
        if (BuildDouble(node, reg, node->a->a, R16_HL, node->b, R8_A)) {
            out.alu_a_reg(node->bi.alu, R8_M);
            return true;
        }
    }
    return false;
}

bool Compiler8080::Case_Operator8_MC(CNodePtr &node, AsmRegister reg) {
    if (node->bi.alu != ALU_NONE && node->b->IsConstNode()) {
        Build(node->a, R8_A);
        out.alu_a_const(node->bi.alu, node->b);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Operator8_MCR(CNodePtr &node, AsmRegister reg) {
    if (node->bi.alu != ALU_NONE && node->a->IsConstNode() && node->bi.rearrange) {
        Build(node->b, R8_A);
        out.alu_a_const(node->bi.alu, node->a);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Shl8_MC(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_SHL && node->b->type == CNT_NUMBER && node->a->ctype.IsUnsigned()) {
        OutShl8(node, reg);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Shr8_MC(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_SHR && node->b->type == CNT_NUMBER && node->a->ctype.IsUnsigned()) {
        OutShr8(node, reg);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Mul8_MC(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_MUL && node->b->type == CNT_NUMBER && node->a->ctype.IsUnsigned()) {
        OutMul8(node->a, node->b, reg);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Mul8_AC(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_MUL && node->b->type == CNT_NUMBER && node->a->bi.alt.able &&
        node->a->ctype.IsUnsigned()) {
        OutMul8(node->a, node->b, R8_D);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Mul8_MCR(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_MUL && node->a->type == CNT_NUMBER && node->a->ctype.IsUnsigned()) {
        OutMul8(node->b, node->a, R8_A);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Mul8_ACR(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_MUL && node->a->type == CNT_NUMBER && node->b->bi.alt.able &&
        node->a->ctype.IsUnsigned()) {
        OutMul8(node->b, node->a, R8_D);
        return true;
    }
    return false;
}
