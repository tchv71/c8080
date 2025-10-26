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
#include "../c/tools/convert.h"
#include "../c/tools/makecnode.h"

// The C parser stores the access to array element (X[Y]) as:
// ARRAY_ELEMENT(X, Y)
// Will be replaced with:
// 1) MONOOPERATOR.DEADDR(OPERATOR.ADD(X, OPERATOR.MUL(SIZEOF, Y))) if struct_item is not array
// 2) OPERATOR.ADD(X, OPERATOR.MUL(SIZEOF, Y)) if struct_item is array

bool PrepareArrayElement(Prepare &p, CNodePtr &node) {
    if (node->type == CNT_MONO_OPERATOR && node->mono_operator_code == MOP_ARRAY_ELEMENT) {
        assert(node->a != nullptr);
        assert(node->b != nullptr);
        assert(node->a->ctype.IsPointer());

        node->type = CNT_OPERATOR;
        node->operator_code = COP_ADD;
        node->b = CNODE({
            CNT_OPERATOR,
            a : Convert(CTYPE_SIZE, node->b),
            b : MakeCNodeNumberSizeT(node->a->ctype.SizeOfElement(node->a->e), node->a->e),
            ctype : CTYPE_SIZE,
            operator_code : COP_MUL,
            e : node->e
        });

        // The result is an array
        // Example:
        //    static int x[5][5];
        //    return x[3];
        std::vector<CPointer> &ap = node->a->ctype.pointers;
        if (ap.size() < 2u || !ap[ap.size() - 2u].is_array) {
            node = MakeCNodeDeaddr(node);
            node->a->ctype.pointers.push_back(CPointer());
        }
        return true;
    }
    return false;
}
