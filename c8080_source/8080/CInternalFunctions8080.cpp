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

#include "CInternalFunctions8080.h"
#include <stdexcept>

static CVariablePtr RegisterInternalFunction(CProgramm &p, const char *name) {
    CVariablePtr v = p.FindVariable(name);
    if (!v)
        throw std::runtime_error(std::string("function ") + name + " not found");
    if (!v->type.IsFunction())
        p.Error(v->e, std::string(name) + " should be a function");
    return v;
}

void CInternalFunctions8080::Init(CProgramm &p) {
    sub_16 = RegisterInternalFunction(p, "__o_sub_16");
    mul_u8 = RegisterInternalFunction(p, "__o_mul_u8");
    mul_i8 = RegisterInternalFunction(p, "__o_mul_i8");
    div_u8 = RegisterInternalFunction(p, "__o_div_u8");
    div_i8 = RegisterInternalFunction(p, "__o_div_i8");
    mod_u8 = RegisterInternalFunction(p, "__o_mod_u8");
    mod_i8 = RegisterInternalFunction(p, "__o_mod_i8");
    shl_8 = RegisterInternalFunction(p, "__o_shl_8");
    shr_u8 = RegisterInternalFunction(p, "__o_shr_u8");
    shr_i8 = RegisterInternalFunction(p, "__o_shr_i8");
    and_16 = RegisterInternalFunction(p, "__o_and_16");
    or_16 = RegisterInternalFunction(p, "__o_or_16");
    xor_16 = RegisterInternalFunction(p, "__o_xor_16");
    mul_u16 = RegisterInternalFunction(p, "__o_mul_u16");
    mul_i16 = RegisterInternalFunction(p, "__o_mul_i16");
    div_u16 = RegisterInternalFunction(p, "__o_div_u16");
    div_i16 = RegisterInternalFunction(p, "__o_div_i16");
    mod_u16 = RegisterInternalFunction(p, "__o_mod_u16");
    mod_i16 = RegisterInternalFunction(p, "__o_mod_i16");
    shl_16 = RegisterInternalFunction(p, "__o_shl_16");
    shl_16 = RegisterInternalFunction(p, "__o_shl_16");
    shr_u16 = RegisterInternalFunction(p, "__o_shr_u16");
    shr_i16 = RegisterInternalFunction(p, "__o_shr_i16");
    add_32 = RegisterInternalFunction(p, "__o_add_32");
    sub_32 = RegisterInternalFunction(p, "__o_sub_32");
    minus_16 = RegisterInternalFunction(p, "__o_minus_16");
    neg_16 = RegisterInternalFunction(p, "__o_neg_16");
    call_hl = RegisterInternalFunction(p, "__o_call_hl");
    i8_to_i16 = RegisterInternalFunction(p, "__o_i8_to_i16");
    i16_to_i32 = RegisterInternalFunction(p, "__o_i16_to_i32");
    load_32 = RegisterInternalFunction(p, "__o_load_32");
    set_32 = RegisterInternalFunction(p, "__o_set_32");
    and_32 = RegisterInternalFunction(p, "__o_and_32");
    or_32 = RegisterInternalFunction(p, "__o_or_32");
    xor_32 = RegisterInternalFunction(p, "__o_xor_32");
    mul_u32 = RegisterInternalFunction(p, "__o_mul_u32");
    mul_i32 = RegisterInternalFunction(p, "__o_mul_i32");
    div_u32 = RegisterInternalFunction(p, "__o_div_u32");
    div_i32 = RegisterInternalFunction(p, "__o_div_i32");
    mod_u32 = RegisterInternalFunction(p, "__o_mod_u32");
    mod_i32 = RegisterInternalFunction(p, "__o_mod_i32");
    shl_32 = RegisterInternalFunction(p, "__o_shl_32");
    shr_u32 = RegisterInternalFunction(p, "__o_shr_u32");
    shr_i32 = RegisterInternalFunction(p, "__o_shr_i32");
    minus_32 = RegisterInternalFunction(p, "__o_minus_32");
    neg_32 = RegisterInternalFunction(p, "__o_neg_32");
}
