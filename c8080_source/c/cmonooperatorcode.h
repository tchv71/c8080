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

enum CMonoOperatorCode {
    MOP_INC,
    MOP_DEC,
    MOP_PLUS,
    MOP_MINUS,
    MOP_NOT,
    MOP_NEG,
    MOP_DEADDR,
    MOP_ADDR,
    MOP_POST_INC,
    MOP_POST_DEC,
    MOP_ARRAY_ELEMENT,
    MOP_STRUCT_ITEM,
    MOP_STRUCT_ITEM_POINTER,
};

const char *ToString(CMonoOperatorCode operator_code);
