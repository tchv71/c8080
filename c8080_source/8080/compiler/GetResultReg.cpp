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

namespace I8080 {

AsmRegister Compiler::GetResultReg(CType &type, bool alt, bool no_result, CNodePtr &e) {
    if (no_result)
        return REG_NONE;
    switch (type.GetAsmType()) {
        case CBT_VOID:
            return REG_NONE;
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
            return alt ? R8_D : R8_A;
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
            return alt ? R16_DE : R16_HL;
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
            if (alt)
                C_ERROR_INTERNAL(e, "no alternative register for 32-bit type");
            return R32_DEHL;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE_INT(e, type.GetAsmType());
            return REG_NONE;
    }
}

}  // namespace I8080
