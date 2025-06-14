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

#include "../cerrorposition.h"
#include "../cnodeptr.h"

void CThrow(CConstErrorPosition position, CString text);
void CThrow(CConstNodePtr node, CString text);
void CInternalError(CConstNodePtr node, CString text, const char *file_name, unsigned line);

// Throw an internal compiler error exception, including the position in the compiler source code

#define C_INTERNAL_ERROR(NODE, TEXT) CInternalError((NODE), (TEXT), __PRETTY_FUNCTION__, __LINE__)
