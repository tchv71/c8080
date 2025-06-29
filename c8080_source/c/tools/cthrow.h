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

void CThrow(const CErrorPosition &position, CString text);
void CThrow(const CNodePtr &node, CString text);
void CInternalError(const CErrorPosition &position, CString text, const char *file_name, unsigned line);
void CInternalError(const CNodePtr &node, CString text, const char *file_name, unsigned line);

// Throw an internal compiler error exception, including the position in the compiler source code

#define C_ERROR_INTERNAL(NODE, TEXT) CInternalError((NODE), (TEXT), __PRETTY_FUNCTION__, __LINE__)

#define C_ERROR_UNSUPPORTED_NODE_TYPE(NODE) \
    C_ERROR_INTERNAL((NODE), std::string("unsupported node type ") + ToString((NODE)->type))

#define C_ERROR_UNSUPPORTED_ASM_TYPE(ASMTYPE, NODE) \
    C_ERROR_INTERNAL((NODE), std::string("unsupported data type ") + CType{ASMTYPE}.ToString())

#define C_ERROR_UNSUPPORTED_OPERATOR(NODE) \
    C_ERROR_INTERNAL((NODE), std::string("unsupported operator ") + ToString((NODE)->operator_code))

#define C_ERROR_CONVERSION(NODE, A, B) \
    C_ERROR_INTERNAL((NODE), "Unsupported conversion from " + CType{(A)}.ToString() + " to " + CType{(B)}.ToString())
