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

#include "cthrow.h"
#include "../cnode.h"
#include <stdexcept>

// Throw an exception including the position in the source code

void CThrow(CConstNodePtr node, CString text) {
    CThrow(node->e, text);
}

// Throw an exception including the position in the source code

void CThrow(const CErrorPosition &position, CString text) {
    throw std::runtime_error(position.ToString() + ": " + text);
}

// Throw an internal compiler error exception, including the position in the compiler source code

void CThrowTypeNotSupportedInternal(CConstNodePtr node, const char *file_name, unsigned line) {
    CThrow(node, "Internal error, type " + node->ctype.ToString() + " is not supported in " + file_name + " " +
                     std::to_string(line));
}
