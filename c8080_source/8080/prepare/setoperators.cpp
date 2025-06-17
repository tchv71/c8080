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

static bool SetOperators(CNodePtr &node, COperatorCode op) {
    if (node->a->type != CNT_MONO_OPERATOR || node->a->mono_operator_code != MOP_DEADDR)
        CThrow(node, "lvalue required as left operand of assignment");  // gcc

    if (node->a->a->IsConstNode()) {
        // Replace
        // SELF_ADD(DEADDR(const), x)
        // with
        // SET(DEADDR(const), ADD(DEADDR(const), x))
        node->b = CNODE({
            CNT_OPERATOR,
            a : CopyNode(node->a),
            b : node->b,
            ctype : node->a->ctype,
            operator_code : op,
            e : node->e
        });
        node->type = CNT_OPERATOR;
        node->operator_code = COP_SET;
        return true;
    }

    // Replace
    // SELF_ADD(DEADDR(x), y)
    // with
    // SET(x, ADD(DEADDR(LOAD_FROM_REGISTER), y))

    CNodePtr reg = CNODE({CNT_LOAD_FROM_REGISTER, ctype : CTYPE_SIZE, e : node->e});
    reg->compiler.hl_contains_value = true;

    CNodePtr da =
        CNODE({CNT_MONO_OPERATOR, a : reg, ctype : node->a->ctype, mono_operator_code : MOP_DEADDR, e : node->e});
    da->compiler.hl_contains_value = true;

    node->b = CNODE({CNT_OPERATOR, a : da, b : node->b, ctype : node->a->ctype, operator_code : op, e : node->e});

    node->type = CNT_SET_OPERATION;
    node->compiler.hl_contains_value = true;

    DeleteNode(node->a, 'a');  // Remove DEADDR

    return true;
}

bool Prepare8080SetOperators(Prepare &, CNodePtr &node) {
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
