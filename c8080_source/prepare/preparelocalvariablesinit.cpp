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

#include "prepareint.h"

// The C parser stores the initialization of local variables as:
// DECLARE_VARIABLE(variable)
// Must be replaced with:
// OPERATOR.SET(LOAD_VARIABLE(variable), variable->body)

bool PrepareLocalVariablesInit(CNodePtr &node) {
    if (node->type == CNT_DECLARE_VARIABLE && node->variable->body != NULL && !node->variable->type.flag_static) {
        node->type = CNT_OPERATOR;
        node->operator_code = COP_SET;
        node->a = CNODE(CNT_LOAD_VARIABLE, ctype : node->ctype, variable : node->variable, e : node->e);
        node->b = node->variable->body;
        node->variable->body = nullptr;
        return true;
    }
    return false;
}
