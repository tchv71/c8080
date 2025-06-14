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
#include "../c/tools/getnumberasuint64.h"

// Replace
// CNT_OPERATOR.COP_ADD(CNT_STACK_ADDRESS(A) / CNT_ARG_STACK_ADDRESS(A), B).
// with:
// CNT_STACK_ADDRESS(A + B) / CNT_ARG_STACK_ADDRESS(A + B).

// Operation A[B] for stack variables is parsed into ADD(STACK_ADDRESS(A), B).

static inline bool Prepare8080AddWithStackAddress2(CNodePtr &node, CNodePtr stack, CNodePtr number) {
    if (number->type == CNT_NUMBER) {
        if (stack->type == CNT_STACK_ADDRESS || stack->type == CNT_ARG_STACK_ADDRESS) {
            stack->number.u += GetNumberAsUint64(number);
            return true;
        }
#if 0
        if (stack->type == CNT_CONVERT &&
            (stack->a->type == CNT_STACK_ADDRESS || stack->a->type == CNT_ARG_STACK_ADDRESS)) {
            stack->a->number.u += GetNumberAsUint64(number);
            return true;
        }
#endif
    }
    return false;
}

bool PrepareAddWithStackAddress(Prepare &, CNodePtr &node) {
    if (node->type == CNT_OPERATOR && node->operator_code == COP_ADD) {
        if (Prepare8080AddWithStackAddress2(node, node->a, node->b)) {
            DeleteNode(node, 'a');
            return true;
        }
        if (Prepare8080AddWithStackAddress2(node, node->b, node->a)) {
            DeleteNode(node, 'b');
            return true;
        }
    }
    return false;
}
