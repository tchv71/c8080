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

namespace I8080 {

void Compiler::OutShr8(CNodePtr &node, AsmRegister reg) {
    assert(node->type == CNT_OPERATOR);
    assert(node->operator_code == COP_SHR);
    assert(node->a != nullptr);
    assert(node->b != nullptr);
    assert(node->a->ctype.Is8BitType());
    assert(node->a->ctype.IsUnsigned());
    assert(node->b->type == CNT_NUMBER);
    assert(reg == R8_A || reg == REG_NONE);

    if (reg == REG_NONE) {
        Build(node->a, REG_NONE);
        return;
    }

    uint8_t value = GetNumberAsUint64(node->b);

    if (value >= 8) {
        Build(node->a, REG_NONE);
        out.ld_a_n8(0);  // This is XOR
        return;
    }

    Build(node->a, R8_A);

    if (value == 0)
        return;

    if (value == 1) {
        out.or_a();  // Clear CF
        out.carry_rotate_right();
        return;
    }

    if (value > 4) {
        for (unsigned i = 0; i < (8u - value); i++)
            out.cyclic_rotate_left();
    } else {
        for (unsigned i = 0; i < value; i++)
            out.cyclic_rotate_right();
    }
    out.and_number(0xFF >> value);
}

}  // namespace I8080
