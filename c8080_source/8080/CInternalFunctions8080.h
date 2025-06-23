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

#pragma once

#include "../c/cprogramm.h"

class CInternalFunctions8080 {
public:
    CVariablePtr call_hl;

    CVariablePtr shl_8;
    CVariablePtr shr_u8;
    CVariablePtr shr_i8;
    CVariablePtr mul_u8;
    CVariablePtr mul_i8;
    CVariablePtr div_u8;
    CVariablePtr div_i8;
    CVariablePtr mod_u8;
    CVariablePtr mod_i8;

    CVariablePtr i8_to_i16;
    CVariablePtr minus_16;
    CVariablePtr neg_16;
    CVariablePtr and_16;
    CVariablePtr sub_16;
    CVariablePtr or_16;
    CVariablePtr xor_16;
    CVariablePtr shl_16;
    CVariablePtr shr_u16;
    CVariablePtr shr_i16;
    CVariablePtr mul_u16;
    CVariablePtr mul_i16;
    CVariablePtr div_u16;
    CVariablePtr div_i16;
    CVariablePtr mod_u16;
    CVariablePtr mod_i16;

    CVariablePtr i16_to_i32;
    CVariablePtr load_32;
    CVariablePtr set_32;
    CVariablePtr minus_32;
    CVariablePtr neg_32;
    CVariablePtr add_32;
    CVariablePtr sub_32;
    CVariablePtr or_32;
    CVariablePtr and_32;
    CVariablePtr xor_32;
    CVariablePtr shl_32;
    CVariablePtr shr_u32;
    CVariablePtr shr_i32;
    CVariablePtr mul_u32;
    CVariablePtr mul_i32;
    CVariablePtr div_u32;
    CVariablePtr div_i32;
    CVariablePtr mod_u32;
    CVariablePtr mod_i32;

    void Init(CProgramm &p);
};
