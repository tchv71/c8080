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
#include "../GetConst.h"

void Compiler8080::BuildConst(CNodePtr &node, AsmRegister reg) {
    assert(node->IsConstNode());

    if (reg == REG_NONE)
        return;

    switch (node->ctype.GetAsmType()) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
            if (reg == REG_PREPARE) {
                node->bi.SetMainAndAlt(U_A, 2, U_D, 2);
                return;
            }
            out.ld_r8_const(reg, node);
            return;
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
            if (reg == REG_PREPARE) {
                node->bi.SetMainAndAlt(U_HL, 3, U_DE, 3);
                return;
            }
            out.ld_r16_const(reg, node);
            return;
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
            if (reg == REG_PREPARE) {
                node->bi.SetMain(U_HLDE, 6);
                return;
            }
            if (node->type == CNT_NUMBER) {
                const uint64_t value = GetNumberAsUint64(node);
                out.ld_r16_number(R16_DE, value >> 16);
                out.ld_r16_number(R16_HL, value);
            } else {
                const std::string value = GetConst(node, nullptr, &out);
                out.ld_r16_string(R16_DE, "(" + value + ") >> 16");
                out.ld_r16_string(R16_HL, "(" + value + ") & 0FFFFh");
            }
            return;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node->ctype.GetAsmType(), node);
    }
}
