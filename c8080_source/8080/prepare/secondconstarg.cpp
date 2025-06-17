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

bool Prepare8080SecondConstArg(Prepare &, CNodePtr &node) {
    assert(node != nullptr);
    if (node->type == CNT_OPERATOR && (node->a->IsConstNode() || node->a->IsDeaddr()) &&
        !(node->b->IsConstNode() || node->b->IsDeaddr())) {
        switch (node->operator_code) {
            case COP_CMP_L:
                std::swap(node->a, node->b);
                node->operator_code = COP_CMP_G;
                return true;
            case COP_CMP_G:
                std::swap(node->a, node->b);
                node->operator_code = COP_CMP_L;
                return true;
            case COP_CMP_LE:
                std::swap(node->a, node->b);
                node->operator_code = COP_CMP_GE;
                return true;
            case COP_CMP_GE:
                std::swap(node->a, node->b);
                node->operator_code = COP_CMP_LE;
                return true;
            case COP_CMP_E:
            case COP_CMP_NE:
            case COP_ADD:
            case COP_MUL:
            case COP_AND:
            case COP_OR:
            case COP_XOR:
                std::swap(node->a, node->b);
                return true;
        }
    }
    return false;
}
