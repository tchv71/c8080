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

void Compiler8080::Case_Convert_void(CNodePtr &node, AsmRegister reg) {
    Build(node->a, REG_NONE);
}

void Compiler8080::Case_Convert_U8_16_MM(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    out.ld_r8_r8(R8_L, R8_A);
    out.ld_r8_number(R8_H, 0);
}

void Compiler8080::Case_Convert_U8_16_AM(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    out.ld_r8_r8(R8_E, R8_A);
    out.ld_r8_number(R8_D, 0);
}

void Compiler8080::Case_Convert_U8_16_MA(CNodePtr &node, AsmRegister reg) {
    if (node->a->bi.alt.able) {
        Build(node->a, R8_D);
        out.ld_r8_r8(R8_L, R8_D);
        out.ld_r8_number(R8_H, 0);
    }
}

void Compiler8080::Case_Convert_U8_16_AA(CNodePtr &node, AsmRegister reg) {
    if (node->a->bi.alt.able) {
        Build(node->a, R8_D);
        out.ld_r8_r8(R8_E, R8_D);
        out.ld_r8_number(R8_D, 0);
    }
}

void Compiler8080::Case_Convert_S8_16(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    InternalCall(o.i8_to_i16);
}

void Compiler8080::Case_Convert_U8_32_M(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    out.ld_r8_r8(R8_L, R8_A);
    out.ld_de_number(0);
    out.ld_h_d();
}

void Compiler8080::Case_Convert_U8_32_A(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_D);
    out.ld_r8_r8(R8_L, R8_D);
    out.ld_de_number(0);
    out.ld_h_d();
}

void Compiler8080::Case_Convert_S8_32(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R8_A);
    InternalCall(o.i8_to_i16);
    InternalCall(o.i16_to_i32);
}

void Compiler8080::Case_Convert_U16_32(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R16_HL);
    out.ld_de_number(0);
}

void Compiler8080::Case_Convert_S16_32(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R16_HL);
    InternalCall(o.i16_to_i32);
}

void Compiler8080::Case_Convert_16_8_M(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R16_HL);
    out.ld_r8_r8(reg, R8_L);
}

void Compiler8080::Case_Convert_16_8_A(CNodePtr &node, AsmRegister reg) {
    if (node->a->bi.alt.able) {
        Build(node->a, R16_DE);
        out.ld_r8_r8(reg, R8_E);
    }
}

void Compiler8080::Case_Convert_32_8(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R32_DEHL);
    out.ld_r8_r8(reg, R8_L);
}

void Compiler8080::Case_Convert_32_16(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R32_DEHL);
    // HL
}

#define PAIR(A, B) (unsigned(A) + (unsigned(B) << 8))

void Compiler8080::BuildConvert(CNodePtr &node, AsmRegister reg) {
    if (MeasureReset(node, reg))
        return;

    Build(node->a, reg);

    Measure(node, REG_NONE, &Compiler8080::Case_Convert_void);

    if (node->ctype.IsVoid())
        return;

    switch (PAIR(node->a->ctype.GetAsmType(), node->ctype.GetAsmType())) {
        // 8 bit -> 16 bit
        case PAIR(CBT_UNSIGNED_CHAR, CBT_SHORT):
        case PAIR(CBT_UNSIGNED_CHAR, CBT_UNSIGNED_SHORT):
            Measure(node, R16_HL, &Compiler8080::Case_Convert_U8_16_MM);
            Measure(node, R16_HL, &Compiler8080::Case_Convert_U8_16_MA);
            Measure(node, R16_DE, &Compiler8080::Case_Convert_U8_16_AM);
            Measure(node, R16_DE, &Compiler8080::Case_Convert_U8_16_AA);
            break;
        // signed 8 bit -> 16 bit
        case PAIR(CBT_CHAR, CBT_SHORT):
        case PAIR(CBT_CHAR, CBT_UNSIGNED_SHORT):
            Measure(node, R16_HL, &Compiler8080::Case_Convert_S8_16);
            break;
        // 8 bit -> 32 bit
        case PAIR(CBT_UNSIGNED_CHAR, CBT_LONG):
        case PAIR(CBT_UNSIGNED_CHAR, CBT_UNSIGNED_LONG):
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_U8_32_M);
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_U8_32_A);
            break;
        // signed 8 bit -> 32 bit
        case PAIR(CBT_CHAR, CBT_LONG):
        case PAIR(CBT_CHAR, CBT_UNSIGNED_LONG):
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_S8_32);
            break;
        // 16 bit -> 32 bit
        case PAIR(CBT_UNSIGNED_SHORT, CBT_LONG):
        case PAIR(CBT_UNSIGNED_SHORT, CBT_UNSIGNED_LONG):
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_U16_32);
            break;
        // signed 16 bit -> 32 bit
        case PAIR(CBT_SHORT, CBT_LONG):
        case PAIR(CBT_SHORT, CBT_UNSIGNED_LONG):
            Measure(node, R32_DEHL, &Compiler8080::Case_Convert_S16_32);
            break;
        // 16 bit -> 8 bit
        case PAIR(CBT_SHORT, CBT_CHAR):
        case PAIR(CBT_SHORT, CBT_UNSIGNED_CHAR):
        case PAIR(CBT_UNSIGNED_SHORT, CBT_CHAR):
        case PAIR(CBT_UNSIGNED_SHORT, CBT_UNSIGNED_CHAR):
            Measure(node, R8_A, &Compiler8080::Case_Convert_16_8_M);
            Measure(node, R8_D, &Compiler8080::Case_Convert_16_8_M);
            Measure(node, R8_A, &Compiler8080::Case_Convert_16_8_A);
            Measure(node, R8_D, &Compiler8080::Case_Convert_16_8_A);
            break;
        // 32 bit -> 8 bit
        case PAIR(CBT_LONG, CBT_CHAR):
        case PAIR(CBT_LONG, CBT_UNSIGNED_CHAR):
        case PAIR(CBT_UNSIGNED_LONG, CBT_CHAR):
        case PAIR(CBT_UNSIGNED_LONG, CBT_UNSIGNED_CHAR):
            Measure(node, R8_A, &Compiler8080::Case_Convert_32_8);
            Measure(node, R8_D, &Compiler8080::Case_Convert_32_8);
            break;
        // 32 bit -> 16 bit
        case PAIR(CBT_LONG, CBT_SHORT):
        case PAIR(CBT_LONG, CBT_UNSIGNED_SHORT):
        case PAIR(CBT_UNSIGNED_LONG, CBT_SHORT):
        case PAIR(CBT_UNSIGNED_LONG, CBT_UNSIGNED_SHORT):
            Measure(node, R16_HL, &Compiler8080::Case_Convert_32_16);
            break;
        default:
            C_ERROR_CONVERSION(node, node->a->ctype.GetAsmType(), node->ctype.GetAsmType());
    }
}
