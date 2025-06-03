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
    MOP_INC,                  // ++a
    MOP_DEC,                  // --a
    MOP_PLUS,                 // +a
    MOP_MINUS,                // -a
    MOP_NOT,                  // ~a
    MOP_NEG,                  // -a
    MOP_DEADDR,               // *a
    MOP_ADDR,                 // &a
    MOP_POST_INC,             // a++
    MOP_POST_DEC,             // a--
    MOP_ARRAY_ELEMENT,        // a[n]
    MOP_STRUCT_ITEM,          // a.n
    MOP_STRUCT_ITEM_POINTER,  // a->n
};

const char *ToString(CMonoOperatorCode operator_code);
