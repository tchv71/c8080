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

bool Compiler8080::OutIncDec16(AsmRegister reg, CNodePtr &a, CNodePtr &b) {
    uint16_t value = GetNumberAsUint64(b);
    if (value <= 3 || value >= 0x10000 - 3) {
        Build(a, reg);
        if (value < 0x8000) {
            for (unsigned i = 0; i < value; i++)
                out.inc_reg(reg);
        } else {
            for (unsigned i = value; i < 0x10000; i++)
                out.dec_reg(reg);
        }
        return true;
    }
    return false;
}
