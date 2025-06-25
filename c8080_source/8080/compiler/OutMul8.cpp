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

void Compiler8080::OutMul8(CNodePtr &var, CNodePtr &number_node, AsmRegister reg) {
    assert(var != nullptr);
    assert(var->ctype.Is8BitType());
    assert(var->ctype.IsUnsigned());
    assert(number_node != nullptr);
    assert(number_node->ctype.IsUnsigned());
    assert(reg == R8_A || reg == R8_D || reg == REG_NONE);

    if (reg == REG_NONE) {
        Build(var, REG_NONE);
        return;
    }

    const uint8_t number = GetNumberAsUint64(number_node);

    if (number == 0) {
        Build(var, REG_NONE);
        out.ld_a_n8(0);  // This is XOR
        return;
    }

    Build(var, reg);

    unsigned programm_size = 0;
    while (uint8_t(number >> programm_size) > 1)
        programm_size++;

    if (reg == R8_D)
        out.ld_r8_r8(R8_A, R8_D);
    else if ((number ^ (1 << programm_size)) != 0)
        out.ld_r8_r8(R8_D, R8_A);

    for (unsigned i = programm_size; i > 0; i--) {
        out.add_a();
        if (number & (1 << (i - 1)))
            out.add_d();
    }
}
