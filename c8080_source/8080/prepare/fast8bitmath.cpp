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

#include "index.h"
#include "../../c/tools/convert.h"

bool Prepare8080Fast8BitMath(Prepare &, CNodePtr &node) {
    // Replace
    // CNT_CONVERT(8_BIT_TYPE, CNT_OPERATOR(A, B))
    // with
    // CNT_OPERATOR(CNT_CONVERT(8_BIT_TYPE, A), CNT_CONVERT(8_BIT_TYPE, B))
    if (node->type == CNT_CONVERT && node->a->type == CNT_OPERATOR && node->ctype.Is8BitType()) {
        switch (node->a->operator_code) {
            // No COP_DIV, COP_MOD. Because convert (1 / 0x1001) to (1 / 1) is incorrect.
            case COP_ADD:
            case COP_SUB:
            case COP_MUL:
            case COP_SHL:
            case COP_AND:
            case COP_OR:
            case COP_XOR:
                node->a->ctype = node->ctype;
                DeleteNode(node, 'a');
                node->a = Convert(node->ctype, node->a);
                node->b = Convert(node->ctype, node->b);
                return true;
        }
    }

    // Replace
    // CNT_CONVERT(8_BIT_TYPE, CNT_OPERATOR(IF, A, B, C))
    // with
    // CNT_OPERATOR(A, CNT_CONVERT(8_BIT_TYPE, B), CNT_CONVERT(8_BIT_TYPE, C))
    if (node->type == CNT_CONVERT && node->a->type == CNT_OPERATOR && node->a->operator_code == COP_IF &&
        node->ctype.Is8BitType()) {
        node->a->ctype = node->ctype;
        DeleteNode(node, 'a');
        node->b = Convert(node->ctype, node->b);
        node->c = Convert(node->ctype, node->c);
        return true;
    }

    return false;
}
