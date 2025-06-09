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

#include "internalfunction.h"

const char *internal_function_names[IF_COUNT] = {
    "__o_sub_16",     "__o_mul_u8",   "__o_mul_u8",  "__o_div_u8",   "__o_div_u8",  "__o_mod_u8",  "__o_mod_u8",
    "__o_shl_8",      "__o_shr_u8",   "__o_shr_i8",  "__o_and_16",   "__o_or_16",   "__o_xor_16",  "__o_mul_u16",
    "__o_mul_i16",    "__o_div_u16",  "__o_div_i16", "__o_mod_u16",  "__o_mod_i16", "__o_shl_16",  "__o_shr_u16",
    "__o_shr_i16",    "__o_add_32",   "__o_sub_32",  "__o_minus_16", "__o_neg_16",  "__o_call_hl", "__o_i8_to_i16",
    "__o_i16_to_i32", "__o_load_32",  "__o_set_32",  "__o_and_32",   "__o_or_32",   "__o_xor_32",  "__o_mul_u32",
    "__o_mul_i32",    "__o_div_u32",  "__o_div_i32", "__o_mod_u32",  "__o_mod_i32", "__o_shl_32",  "__o_shr_u32",
    "__o_shr_i32",    "__o_minus_32", "__o_neg_32",
};

void InternalFunctionDependencies(bool *used) {
    if (used[IF_MUL_I16]) {
        used[IF_MUL_U16] = true;
        used[IF_MINUS_16] = true;
    }
    if (used[IF_DIV_I16]) {
        used[IF_DIV_U16] = true;
        used[IF_MINUS_16] = true;
    }
    if (used[IF_MOD_U8])
        used[IF_DIV_U8] = true;
    if (used[IF_MOD_U16])
        used[IF_DIV_U16] = true;
    if (used[IF_MOD_U32])
        used[IF_DIV_U32] = true;
    if (used[IF_MOD_I8])
        used[IF_DIV_I8] = true;
    if (used[IF_MOD_I16])
        used[IF_DIV_I16] = true;
    if (used[IF_MOD_I32])
        used[IF_DIV_I32] = true;
}
