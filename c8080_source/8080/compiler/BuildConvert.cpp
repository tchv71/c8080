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

#define PAIR(A, B) (unsigned(A) + (unsigned(B) << 8))

void Compiler8080::BuildConvert(CNodePtr &node) {
    assert(node->type == CNT_CONVERT);
    assert(node->a != nullptr);

    Build(node->a);

    Measure(node, REG_NONE, &Compiler8080::Case_Convert_void);

    if (node->ctype.IsVoid())
        return;

    switch (PAIR(node->a->ctype.GetAsmType(), node->ctype.GetAsmType())) {
        case PAIR(CBT_UNSIGNED_CHAR, CBT_SHORT):
        case PAIR(CBT_UNSIGNED_CHAR, CBT_UNSIGNED_SHORT):
            Measure(node, R16_HL, &Compiler8080::Case_Convert_U8_16_MM);
            Measure(node, R16_HL, &Compiler8080::Case_Convert_U8_16_MA);
            Measure(node, R16_DE, &Compiler8080::Case_Convert_U8_16_AM);
            Measure(node, R16_DE, &Compiler8080::Case_Convert_U8_16_AA);
            Measure(node, R16_HL, &Compiler8080::Case_ConvertLoadConstAddr_U8_16);
            Measure(node, R16_HL, &Compiler8080::Case_ConvertLoad_U8_16);
            Measure(node, R16_DE, &Compiler8080::Case_ConvertLoad_U8_16);
            break;
        case PAIR(CBT_CHAR, CBT_SHORT):
        case PAIR(CBT_CHAR, CBT_UNSIGNED_SHORT):
            Measure(node, R16_HL, &Compiler8080::Case_Convert_S8_16);
            break;
        case PAIR(CBT_UNSIGNED_CHAR, CBT_LONG):
        case PAIR(CBT_UNSIGNED_CHAR, CBT_UNSIGNED_LONG):
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_U8_32_M);
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_U8_32_A);
            Measure(node, R32_DEHL, &Compiler8080::Case_ConvertLoadConstAddr_U8_32);
            Measure(node, R32_DEHL, &Compiler8080::Case_ConvertLoad_U8_32);
            break;
        case PAIR(CBT_CHAR, CBT_LONG):
        case PAIR(CBT_CHAR, CBT_UNSIGNED_LONG):
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_S8_32);
            break;
        case PAIR(CBT_UNSIGNED_SHORT, CBT_LONG):
        case PAIR(CBT_UNSIGNED_SHORT, CBT_UNSIGNED_LONG):
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_U16_32);
            break;
        case PAIR(CBT_SHORT, CBT_LONG):
        case PAIR(CBT_SHORT, CBT_UNSIGNED_LONG):
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_S16_32);
            break;
        case PAIR(CBT_SHORT, CBT_CHAR):
        case PAIR(CBT_SHORT, CBT_UNSIGNED_CHAR):
        case PAIR(CBT_UNSIGNED_SHORT, CBT_CHAR):
        case PAIR(CBT_UNSIGNED_SHORT, CBT_UNSIGNED_CHAR):
            Measure(node, R8_A, &Compiler8080::Case_Convert_16_8_M);
            Measure(node, R8_D, &Compiler8080::Case_Convert_16_8_M);
            Measure(node, R8_A, &Compiler8080::Case_Convert_16_8_A);
            Measure(node, R8_D, &Compiler8080::Case_Convert_16_8_A);
            Measure(node, R8_A, &Compiler8080::Case_ConvertLoadConstAddr_8);
            Measure(node, R8_A, &Compiler8080::Case_ConvertLoad_8);
            Measure(node, R8_D, &Compiler8080::Case_ConvertLoad_8);
            Measure(node, R8_A, &Compiler8080::Case_ConvertLoad_8_A);
            break;
        case PAIR(CBT_LONG, CBT_CHAR):
        case PAIR(CBT_LONG, CBT_UNSIGNED_CHAR):
        case PAIR(CBT_UNSIGNED_LONG, CBT_CHAR):
        case PAIR(CBT_UNSIGNED_LONG, CBT_UNSIGNED_CHAR):
            Measure(node, R8_A, &Compiler8080::Case_Convert_32_8);
            Measure(node, R8_D, &Compiler8080::Case_Convert_32_8);
            Measure(node, R8_A, &Compiler8080::Case_ConvertLoadConstAddr_8);
            Measure(node, R8_A, &Compiler8080::Case_ConvertLoad_8);
            Measure(node, R8_D, &Compiler8080::Case_ConvertLoad_8);
            Measure(node, R8_A, &Compiler8080::Case_ConvertLoad_8_A);
            break;
        case PAIR(CBT_LONG, CBT_SHORT):
        case PAIR(CBT_LONG, CBT_UNSIGNED_SHORT):
        case PAIR(CBT_UNSIGNED_LONG, CBT_SHORT):
        case PAIR(CBT_UNSIGNED_LONG, CBT_UNSIGNED_SHORT):
            Measure(node, R16_HL, &Compiler8080::Case_Convert_32_16);
            Measure(node, R16_HL, &Compiler8080::Case_ConvertLoadConstAddr_16);
            Measure(node, R16_HL, &Compiler8080::Case_ConvertLoad_16);
            Measure(node, R16_DE, &Compiler8080::Case_ConvertLoad_16);
            break;
        default:
            C_ERROR_CONVERSION(node, node->a->ctype.GetAsmType(), node->ctype.GetAsmType());
    }
}

bool Compiler8080::Case_Convert_void(CNodePtr &node, AsmRegister reg) {
    Build(node->a, REG_NONE);
    return true;
}

bool Compiler8080::Case_ConvertLoadConstAddr_8(CNodePtr &node, AsmRegister reg) {
    assert(reg == R8_A);
    if (node->a->IsDeaddr() && node->a->a->IsConstNode()) {
        out.ld_a_pconst(node->a->a);
        return true;
    }
    return false;
}

bool Compiler8080::Case_ConvertLoad_8(CNodePtr &node, AsmRegister reg) {
    assert(reg == R8_A || reg == R8_D);
    if (node->a->IsDeaddr()) {
        Build(node->a->a, R16_HL);
        out.ld_r8_r8(reg, R8_M);
        return true;
    }
    return false;
}

bool Compiler8080::Case_ConvertLoad_8_A(CNodePtr &node, AsmRegister reg) {
    assert(reg == R8_A);
    if (node->a->IsDeaddr() && node->a->a->bi.alt.able) {
        Build(node->a->a, R16_DE);
        out.ld_a_pde();
        return true;
    }
    return false;
}

bool Compiler8080::Case_Convert_U8_16_MM(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    out.ld_r8_r8(R8_L, R8_A);
    out.ld_r8_number(R8_H, 0);
    return true;
}

bool Compiler8080::Case_Convert_U8_16_AM(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    out.ld_r8_r8(R8_E, R8_A);
    out.ld_r8_number(R8_D, 0);
    return true;
}

bool Compiler8080::Case_Convert_U8_16_MA(CNodePtr &node, AsmRegister reg) {
    if (node->a->bi.alt.able) {
        Build(node->a, R8_D);
        out.ld_r8_r8(R8_L, R8_D);
        out.ld_r8_number(R8_H, 0);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Convert_U8_16_AA(CNodePtr &node, AsmRegister reg) {
    if (node->a->bi.alt.able) {
        Build(node->a, R8_D);
        out.ld_r8_r8(R8_E, R8_D);
        out.ld_r8_number(R8_D, 0);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Convert_S8_16(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    InternalCall(o.i8_to_i16);
    return true;
}

bool Compiler8080::Case_Convert_U8_32_M(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    out.ld_r8_r8(R8_L, R8_A);
    out.ld_de_number(0);
    out.ld_h_d();
    return true;
}

bool Compiler8080::Case_Convert_U8_32_A(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_D);
    out.ld_r8_r8(R8_L, R8_D);
    out.ld_de_number(0);
    out.ld_h_d();
    return true;
}

bool Compiler8080::Case_Convert_S8_32(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    InternalCall(o.i8_to_i16);
    InternalCall(o.i16_to_i32);
    return true;
}

bool Compiler8080::Case_Convert_U16_32(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R16_HL);
    out.ld_de_number(0);
    return true;
}

bool Compiler8080::Case_Convert_S16_32(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R16_HL);
    InternalCall(o.i16_to_i32);
    return true;
}

bool Compiler8080::Case_Convert_16_8_M(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R16_HL);
    out.ld_r8_r8(reg, R8_L);
    return true;
}

bool Compiler8080::Case_Convert_16_8_A(CNodePtr &node, AsmRegister reg) {
    if (node->a->bi.alt.able) {
        Build(node->a, R16_DE);
        out.ld_r8_r8(reg, R8_E);
        return true;
    }
    return false;
}

bool Compiler8080::Case_Convert_32_8(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R32_DEHL);
    out.ld_r8_r8(reg, R8_L);
    return true;
}

bool Compiler8080::Case_Convert_32_16(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R32_DEHL);
    // HL
    return true;
}

bool Compiler8080::Case_ConvertLoadConstAddr_16(CNodePtr &node, AsmRegister reg) {
    assert(reg == R16_HL);
    if (node->a->IsDeaddr() && node->a->a->IsConstNode()) {
        out.ld_hl_pconst(node->a->a);
        return true;
    }
    return false;
}

bool Compiler8080::Case_ConvertLoad_16(CNodePtr &node, AsmRegister reg) {
    assert(reg == R16_HL || reg == R16_DE);
    if (node->a->IsDeaddr()) {
        Build(node->a->a, R16_HL);
        out.ld_e_phl();
        out.inc_hl();
        out.ld_d_phl();
        if (reg == R16_HL)
            out.ex_hl_de();
        return true;
    }
    return false;
}

bool Compiler8080::Case_ConvertLoadConstAddr_U8_16(CNodePtr &node, AsmRegister reg) {
    assert(reg == R16_HL);
    if (node->a->IsDeaddr() && node->a->a->IsConstNode()) {
        out.ld_hl_pconst(node->a->a);
        out.ld_h_number(0);
        return true;
    }
    return false;
}

bool Compiler8080::Case_ConvertLoad_U8_16(CNodePtr &node, AsmRegister reg) {
    assert(reg == R16_HL || reg == R16_DE);
    if (node->a->IsDeaddr()) {
        Build(node->a->a, R16_HL);
        if (reg == R16_HL) {
            out.ld_l_phl();
            out.ld_h_number(0);
        } else {
            out.ld_e_phl();
            out.ld_d_number(0);
        }
        return true;
    }
    return false;
}

bool Compiler8080::Case_ConvertLoadConstAddr_U8_32(CNodePtr &node, AsmRegister reg) {
    assert(reg == R32_DEHL);
    if (node->a->IsDeaddr() && node->a->a->IsConstNode()) {
        out.ld_hl_pconst(node->a->a);
        out.ld_de_number(0);
        out.ld_h_d();
        return true;
    }
    return false;
}

bool Compiler8080::Case_ConvertLoad_U8_32(CNodePtr &node, AsmRegister reg) {
    assert(reg == R32_DEHL);
    if (node->a->IsDeaddr()) {
        Build(node->a->a, R16_HL);
        out.ld_l_phl();
        out.ld_de_number(0);
        out.ld_h_d();
        return true;
    }
    return false;
}
