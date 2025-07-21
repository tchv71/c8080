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
#include "../../c/tools/cthrow.h"

namespace I8080 {

// TODO: Замена ADD на INC  ( (*word)++ )

static bool SetOperators(CNodePtr &node, COperatorCode op) {
    if (node->a->type != CNT_MONO_OPERATOR || node->a->mono_operator_code != MOP_DEADDR)
        CThrow(node, "lvalue required as left operand of assignment");  // gcc

    // TODO: Copy DEADDR(y) to the temp variable

    // Replace
    // SELF_ADD(DEADDR(y), x)
    // with
    // SET(DEADDR(y), ADD(DEADDR(y), x))
    node->b = CNODE(
        {CNT_OPERATOR, a : CopyNode(node->a), b : node->b, ctype : node->a->ctype, operator_code : op, e : node->e});
    node->type = CNT_OPERATOR;
    node->operator_code = COP_SET;
    return true;
}

bool PrepareSetOperators(Prepare &, CNodePtr &node) {
    assert(node != nullptr);
    if (node->type == CNT_OPERATOR) {
        switch (node->operator_code) {
            case COP_SET_ADD:
                return SetOperators(node, COP_ADD);
            case COP_SET_SUB:
                return SetOperators(node, COP_SUB);
            case COP_SET_MUL:
                return SetOperators(node, COP_MUL);
            case COP_SET_DIV:
                return SetOperators(node, COP_DIV);
            case COP_SET_MOD:
                return SetOperators(node, COP_MOD);
            case COP_SET_SHR:
                return SetOperators(node, COP_SHR);
            case COP_SET_SHL:
                return SetOperators(node, COP_SHL);
            case COP_SET_AND:
                return SetOperators(node, COP_AND);
            case COP_SET_OR:
                return SetOperators(node, COP_OR);
            case COP_SET_XOR:
                return SetOperators(node, COP_XOR);
        }
    }
    return false;
}

}  // namespace I8080
