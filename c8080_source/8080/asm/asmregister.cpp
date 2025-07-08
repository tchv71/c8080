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

#include "asmregister.h"
#include <array>
#include <stdexcept>

const char *ToString(AsmRegister value) {
    static const char *strings[] = {"?", "a", "b", "c", "d", "e", "h", "l", "(hl)", "af", "bc", "de", "hl", "sp"};

    if (value >= std::size(strings))
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " " + std::to_string(int(value)));

    return strings[value];
}

uint32_t RegToUsed(AsmRegister reg) {
    switch (reg) {
        case R8_A:
            return U_A;
        case R8_B:
            return U_B;
        case R8_C:
            return U_C;
        case R8_D:
            return U_D;
        case R8_E:
            return U_E;
        case R8_H:
            return U_H;
        case R8_L:
            return U_L;
        case R16_AF:
            return U_A;
        case R16_BC:
            return U_BC;
        case R16_DE:
            return U_DE;
        case R16_HL:
            return U_HL;
        case R32_DEHL:
            return U_DEHL;
    }
    return 0;
}

bool IsAsmRegister16SpAf(AsmRegister reg) {
    switch (reg) {
        case R16_AF:
        case R16_BC:
        case R16_DE:
        case R16_HL:
        case R16_SP:
            return true;
    }
    return false;
}

bool IsAsmRegister16Sp(AsmRegister r) {
    switch (r) {
        case R16_BC:
        case R16_DE:
        case R16_HL:
        case R16_SP:
            return true;
    }
    return false;
}

bool IsAsmRegister8(AsmRegister r) {
    switch (r) {
        case R8_A:
        case R8_B:
        case R8_C:
        case R8_D:
        case R8_E:
        case R8_H:
        case R8_L:
        case R8_M:
            return true;
    }
    return false;
}
