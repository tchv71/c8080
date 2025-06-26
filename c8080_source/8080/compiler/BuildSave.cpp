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

void Compiler8080::Case_Save8_MM(CNodePtr &node, AsmRegister reg) {
    if (BuildArgs(node, reg, node->a->a, R16_HL, node->b, R8_A))
        out.ld_phl_a();
}

void Compiler8080::Case_Save8_AM(CNodePtr &node, AsmRegister reg) {
    if (BuildArgs(node, reg, node->a->a, R16_DE, node->b, R8_A))
        out.ld_pde_a();
}

void Compiler8080::Case_Save8_MA(CNodePtr &node, AsmRegister reg) {
    if (BuildArgs(node, reg, node->a->a, R16_HL, node->b, R8_D)) {
        out.ld_phl_d();
        MeasureMid(node, R8_D, &Compiler8080::Case_Save8_MA);
        MeasureMid(node, REG_NONE, &Compiler8080::Case_Save8_MA);
        if (reg != R8_D)
            out.ld_a_d();
    }
}

void Compiler8080::Case_Save8_MN(CNodePtr &node, AsmRegister reg) {
    if (node->b->IsConstNode()) {
        Build(node->a->a, R16_HL);
        out.ld_phl_const(node->b);
    }
}

void Compiler8080::Case_Save8_MNR(CNodePtr &node, AsmRegister reg) {
    if (node->a->a->IsConstNode()) {
        Build(node->b, R8_A);
        out.ld_pconst_a(node->a->a);
    }
}

void Compiler8080::Case_Save16_AA(CNodePtr &node, AsmRegister reg) {
    BuildArgs2(node, reg, node->a->a, node->b, R16_HL, R16_DE, false);  // No swap
    out.ld_phl_e();
    out.inc_hl();
    out.ld_phl_d();
    MeasureMid(node, R16_DE, &Compiler8080::Case_Save16_AA);
    MeasureMid(node, REG_NONE, &Compiler8080::Case_Save16_AA);
    if (reg != R16_DE)
        out.ex_hl_de();
}

void Compiler8080::Case_Save16_NM(CNodePtr &node, AsmRegister reg) {
    if (node->a->a->IsConstNode()) {
        Build(node->b, R16_HL);
        out.ld_pconst_hl(node->a->a);
    }
}

void Compiler8080::Case_Save32_MM(CNodePtr &node, AsmRegister reg) {
    Build(node->a->a, R16_HL);
    out.push_hl();
    Build(node->b, R32_DEHL);
    InternalCall(o.set_32);
    out.stack_correction(-2);
}

void Compiler8080::Case_Save32_AM(CNodePtr &node, AsmRegister reg) {
    if (node->a->a->bi.alt.able) {
        Build(node->a->a, R16_DE);
        out.push_de();
        Build(node->b, R32_DEHL);
        InternalCall(o.set_32);
        out.stack_correction(-2);
    }
}

void Compiler8080::Case_Save32_NM(CNodePtr &node, AsmRegister reg) {
    if (node->a->a->IsConstNode()) {
        Build(node->b, R32_DEHL);
        out.ld_pconst_dehl_xchg(node->a->a);
        MeasureMid(node, REG_NONE, &Compiler8080::Case_Save32_NM);
        if (reg != REG_NONE)
            out.ex_hl_de();
    }
}

void Compiler8080::BuildSave(CNodePtr &node, AsmRegister reg) {
    if (!node->a->IsDeaddr()) {
        p.Error(node->e, "lvalue required as left operand of assignment");  // gcc
        return;
    }

    if (MeasureReset(node, reg))
        return;

    Build(node->a);
    Build(node->b);

    switch (node->b->ctype.GetAsmType()) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
            Measure(node, R8_A, &Compiler8080::Case_Save8_MM);      // MOV M, A
            Measure(node, R8_A, &Compiler8080::Case_Save8_AM);      // STAX D
            Measure(node, R8_A, &Compiler8080::Case_Save8_MA);      // MOV M, D
            Measure(node, R8_A, &Compiler8080::Case_Save8_MNR);     // STA const
            Measure(node, REG_NONE, &Compiler8080::Case_Save8_MN);  // MVI M, const
            break;
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
            Measure(node, R16_HL, &Compiler8080::Case_Save16_AA);  // MOV M, E / INX H / MOV M, D
            Measure(node, R16_HL, &Compiler8080::Case_Save16_NM);  // SHLD const
            break;
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
            Measure(node, R32_DEHL, &Compiler8080::Case_Save32_MM);  // PUSH HL / CALL set_32
            Measure(node, R32_DEHL, &Compiler8080::Case_Save32_AM);  // PUSH DE / CALL set_32
            Measure(node, R32_DEHL, &Compiler8080::Case_Save32_NM);  // SHLD const / XCHF / SHLD const + 2
            break;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node->b->ctype.GetAsmType(), node);
    }
}
