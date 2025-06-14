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
#include "../c/tools/cthrow.h"
#include "../c/consts.h"

// The C parser stores the access to structure field (X.struct_item) as:
// STRUCT_ITEM(X, struct_item)
// Will be replaced with:
// 1) MONOOPERATOR.DEADDR(OPERATOR.ADD(MONOOPERATOR.ADDR(X), struct_item->struct_item_offset)) if item is not array
// 2) OPERATOR.ADD(MONOOPERATOR.ADDR(X), struct_item->struct_item_offset) if struct_item is array
//
// The C parser stores the access to structure field (X->struct_item) as:
// STRUCT_ITEM_POINTER(X, struct_item)
// Will be replaced with:
// 1) MONOOPERATOR.DEADDR(OPERATOR.ADD(X, struct_item->struct_item_offset)) if struct_item is not array
// 2) OPERATOR.ADD(X, struct_item->struct_item_offset) if struct_item is array

bool PrepareStructItem(Prepare &p, CNodePtr &node) {
    if (node->type == CNT_MONO_OPERATOR &&
        (node->mono_operator_code == MOP_STRUCT_ITEM || node->mono_operator_code == MOP_STRUCT_ITEM_POINTER)) {
        assert(node->a != nullptr);

        CStructItemPtr &si = node->struct_item;
        if (si == nullptr)
            CThrow(node, "Internal error, null pointer in " + std::string(__PRETTY_FUNCTION__));

        if (node->mono_operator_code == MOP_STRUCT_ITEM)
            node->a = MakeCNodeMonoOperatorAddr(node->a);

        assert(node->a->ctype.IsPointer());

        node->type = CNT_OPERATOR;
        node->operator_code = COP_ADD;
        assert(node->b == nullptr);
        node->b = MakeCNodeNumberSize(si->struct_item_offset, node->e);

        if (si->type.pointers.empty() || si->type.pointers.back().count == 0) {
            node = MakeCNodeMonoOperatorDeaddr(node);
            node->a->ctype.pointers.push_back(CPointer{});
        }
        return true;
    }
    return false;
}
