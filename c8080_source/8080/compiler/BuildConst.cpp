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
#include "../../c/tools/getnumberasuint64.h"

void Compiler8080::Case_Const8(CNodePtr &node, AsmRegister reg) {
    out.ld_r8_const(reg, node);
}

void Compiler8080::Case_Const16(CNodePtr &node, AsmRegister reg) {
    out.ld_r16_const(reg, node);
}

void Compiler8080::Case_Const32(CNodePtr &node, AsmRegister reg) {
    out.ld_dehl_const(node);
}

void Compiler8080::BuildConst(CNodePtr &node, AsmRegister reg) {
    assert(node->IsConstNode());

    if (reg == REG_NONE) {
        GetConst(node, nullptr, &out);  // ref counter
        return;
    }

    if (MeasureReset(node, reg))
        return;

    switch (node->ctype.GetAsmType()) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
            Measure(node, R8_A, &Compiler8080::Case_Const8);
            Measure(node, R8_D, &Compiler8080::Case_Const8);
            break;
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
            Measure(node, R16_HL, &Compiler8080::Case_Const16);
            Measure(node, R16_DE, &Compiler8080::Case_Const16);
            break;
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
            Measure(node, R32_DEHL, &Compiler8080::Case_Const32);
            break;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node->ctype.GetAsmType(), node);
    }
}
