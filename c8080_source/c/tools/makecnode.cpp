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

#include "makecnode.h"
#include "../cnode.h"
#include "../consts.h"

CNodePtr MakeCNodeAddr(CNodePtr a) {
    a = CNODE({CNT_MONO_OPERATOR, a : a, ctype : a->ctype, mono_operator_code : MOP_ADDR, e : a->e});
    a->ctype.pointers.push_back(CPointer());
    std::swap(a->next_node, a->a->next_node);
    return a;
}

CNodePtr MakeCNodeDeaddr(CNodePtr a) {
    a = CNODE({CNT_MONO_OPERATOR, a : a, ctype : a->ctype, mono_operator_code : MOP_DEADDR, e : a->e});
    std::swap(a->next_node, a->a->next_node);
    return a;
}

CNodePtr MakeCNodeNumberSizeT(uint64_t number, CErrorPosition &e) {
    CNodePtr node = CNODE({CNT_NUMBER, e : e});
    node->ctype.base_type = CBT_SIZE;
    node->number.u = number & C_SIZE_MAX;  // TODO: Show warning
    return node;
}
