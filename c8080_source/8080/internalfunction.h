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

enum InternalFunction {
    IF_SUB_16,
    IF_MUL_U8,
    IF_MUL_I8,
    IF_DIV_U8,
    IF_DIV_I8,
    IF_MOD_U8,
    IF_MOD_I8,
    IF_SHL_8,
    IF_SHR_U8,
    IF_SHR_I8,
    IF_AND_16,
    IF_OR_16,
    IF_XOR_16,
    IF_MUL_U16,
    IF_MUL_I16,
    IF_DIV_U16,
    IF_DIV_I16,
    IF_MOD_U16,
    IF_MOD_I16,
    IF_SHL_16,
    IF_SHR_U16,
    IF_SHR_I16,
    IF_ADD_32,
    IF_SUB_32,
    IF_MINUS_16,
    IF_NEG_16,
    IF_CALL_HL,
    IF_I8_TO_I16,
    IF_I16_TO_I32,
    IF_LOAD_32,
    IF_SET_32,
    IF_AND_32,
    IF_OR_32,
    IF_XOR_32,
    IF_MUL_U32,
    IF_MUL_I32,
    IF_DIV_U32,
    IF_DIV_I32,
    IF_MOD_U32,
    IF_MOD_I32,
    IF_SHL_32,
    IF_SHR_U32,
    IF_SHR_I32,
    IF_MINUS_32,
    IF_NEG_32,
    IF_COUNT,
};

extern const char *internal_function_names[IF_COUNT];
