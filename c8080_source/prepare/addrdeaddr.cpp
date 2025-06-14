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

// Replace *&x with x

bool PrepareAddrDeaddr(Prepare &, CNodePtr &node) {
    if (node->type == CNT_MONO_OPERATOR && node->mono_operator_code == MOP_ADDR) {
        assert(node->a != nullptr);
        if (node->a->type == CNT_MONO_OPERATOR && node->a->mono_operator_code == MOP_DEADDR) {
            assert(node->ctype.GetAsmType() == node->a->a->ctype.GetAsmType());
            node->a = node->a->a;
            DeleteNodeSaveType(node, 'a');
            return true;
        }
    }
    return false;
}
