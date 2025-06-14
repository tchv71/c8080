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

#include "getfunctionreturnregister.h"

AsmRegister GetFunctionReturnRegister(CConstType ctype, CompileFlags flags) {
    if (flags & CF_NO_RESULT)
        return REG_NONE;
    switch (ctype.GetAsmType()) {
        case CBT_VOID:
            return REG_NONE;
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
            return R8_A;
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
            return R16_HL;
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
            return R32_DEHL;
        default:
            throw std::runtime_error(__FUNCTION__);
    }
}
