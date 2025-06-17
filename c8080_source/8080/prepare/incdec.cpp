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
#include "../../c/tools/makecnode.h"
#include "../../c/tools/cthrow.h"

static inline void IncDec(CNodePtr &node) {
    const size_t item_size = node->ctype.SizeOfElement(node->e);
    CNodePtr number = Convert(CType{node->ctype.GetAsmType()}, MakeCNodeNumberSizeT(item_size, node->e));

    const bool inc = (node->mono_operator_code == MOP_INC || node->mono_operator_code == MOP_POST_INC);
    const auto op = inc ? COP_ADD : COP_SUB;

    if (node->a->type != CNT_MONO_OPERATOR || node->a->mono_operator_code != MOP_DEADDR)
        CThrow(node, "lvalue required as left operand of assignment");  // gcc

    if (node->a->a->IsConstNode()) {
        // Replace
        // INC(DEADDR(const))
        // with
        // SET(DEADDR(const), ADD(DEADDR(const), item_size))

        CNodePtr a = CopyNode(node->a);
        node->b = CNODE({CNT_OPERATOR, a : a, b : number, ctype : a->ctype, operator_code : op, e : node->e});
        node->type = CNT_OPERATOR;
        node->operator_code = COP_SET;
    } else {
        // Replace
        // INC(DEADDR(x))
        // with
        // SET(x, ADD(LOAD_FROM_REGISTER, item_size));

        CNodePtr reg = CNODE({CNT_LOAD_FROM_REGISTER, ctype : CTYPE_SIZE, e : node->e});

        CNodePtr da =
            CNODE({CNT_MONO_OPERATOR, a : reg, ctype : node->a->ctype, mono_operator_code : MOP_DEADDR, e : node->e});

        node->b = CNODE({CNT_OPERATOR, a : da, b : number, ctype : node->a->ctype, operator_code : op, e : node->e});

        node->type = CNT_SET_OPERATION;

        DeleteNode(node->a, 'a');  // Remove DEADDR
    }

    if (node->mono_operator_code == MOP_POST_INC || node->mono_operator_code == MOP_POST_DEC) {
        const auto back_op = inc ? COP_SUB : COP_ADD;

        CNodePtr n = node->next_node;
        node->next_node = nullptr;

        node = CNODE({
            CNT_OPERATOR,
            a : node,
            b : CopyNode(number),
            ctype : node->ctype,
            next_node : n,
            operator_code : back_op,
            e : node->e
        });
    }
}

bool Prepare8080IncDec(Prepare &, CNodePtr &node) {
    if (node->type == CNT_MONO_OPERATOR) {
        switch (node->mono_operator_code) {
            case MOP_INC:
            case MOP_POST_INC:
            case MOP_DEC:
            case MOP_POST_DEC:
                IncDec(node);
                return true;
        }
    }
    return false;
}
