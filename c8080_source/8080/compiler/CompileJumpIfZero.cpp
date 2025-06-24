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

void Compiler8080::CompileJumpIfZero(CNodePtr &node, bool invert1, bool invert2, AsmLabel *label) {
    Build(node);

    switch (node->ctype.GetAsmType()) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
            Build(node, R8_A);
            out.or_a();
            break;
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
            if (node->bi.alt.able && node->bi.alt.metric <= node->bi.main.metric) {
                Build(node, R16_DE);
                out.ld_a_d();
                out.or_e();
            } else {
                Build(node, R16_HL);
                out.ld_a_h();
                out.or_l();
            }
            break;
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
            Build(node, R32_DEHL);
            out.ld_a_l();
            out.or_h();
            out.or_d();
            out.or_e();
            break;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node->ctype.GetAsmType(), node);
    }

    if (invert2)
        invert1 = !invert1;

    if (invert1)
        out.jnz_label(label);
    else
        out.jz_label(label);
}
