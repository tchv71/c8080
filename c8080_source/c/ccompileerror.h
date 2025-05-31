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

#include "c_tree.h"

void CCompileError(const CErrorPosition &position, CString text);
void CCompileError(ConstCNodePtr node, CString text);

#define TYPE_NOT_SUPPORTED(NODE)                                                                                     \
    CCompileError((NODE), "Type " + (NODE)->ctype.ToString() + " is not supported in " + __PRETTY_FUNCTION__ + " " + \
                              std::to_string(__LINE__))
