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
#include <assert.h>

void Compile8080::CompileJumpIfZero(CNodePtr node, bool jmp_if_not_zero_inital, bool invert_jmp_if_not_zero,
                                    AsmLabel *label) {
    switch (node->ctype.GetAsmType()) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR: {
            const AsmRegister reg = CompileExpression(node);
            assert(reg == R8_A);
            out.or_a_a();
            break;
        }
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT: {
            const AsmRegister reg = CompileExpression(node, CF_CAN_USE_ALT);
            if (reg == R16_HL) {
                out.ld_a_h();
                out.or_a_l();
            } else {
                assert(reg == R16_DE);
                out.ld_a_d();
                out.or_a_e();
            }
            break;
        }
        case CBT_LONG:
        case CBT_UNSIGNED_LONG: {
            const AsmRegister reg = CompileExpression(node);
            assert(reg == R32_DEHL);
            out.ld_a_l();
            out.or_a_h();
            out.or_a_d();
            out.or_a_e();
            break;
        }
        default:
            C_INTERNAL_ERROR(node, "data type not supported");
    }
    if (invert_jmp_if_not_zero ? !jmp_if_not_zero_inital : jmp_if_not_zero_inital)
        out.jnz_label(label);
    else
        out.jz_label(label);
}
