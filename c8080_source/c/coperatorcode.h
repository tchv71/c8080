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

enum COperatorCode {
    OP_CMP_L,  // TODO: Rename to COP_CMP_L
    OP_CMP_G,
    OP_CMP_LE,
    OP_CMP_GE,
    OP_CMP_E,
    OP_CMP_NE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_SHR,
    OP_SHL,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_LAND,
    OP_LOR,
    OP_SET,
    OP_SET_ADD,
    OP_SET_SUB,
    OP_SET_MUL,
    OP_SET_DIV,
    OP_SET_MOD,
    OP_SET_SHR,
    OP_SET_SHL,
    OP_SET_AND,
    OP_SET_OR,
    OP_SET_XOR,
    OP_IF,
    OP_COMMA,
};

bool IsSetOperator(COperatorCode code);
bool IsCompareOperator(COperatorCode code);
const char *ToString(COperatorCode code);
