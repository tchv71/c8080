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
#include "../../c/tools/makeoperator.h"

static bool Prepare8080Jump2(CNodePtr &node) {
    if (node && !node->IsJumpNode()) {
        CNodePtr ch = CNODE({CNT_NUMBER, ctype : node->ctype, e : node->e});
        node = MakeOperator(COP_CMP_NE, node, ch, node->e, false);
        node->dont_replace_jump_node = true;
        assert(node->IsJumpNode());
        return true;
    }
    return false;
}

bool Prepare8080Jump(Prepare &, CNodePtr &node) {
    switch (node->type) {
        case CNT_OPERATOR:
            if (node->operator_code == COP_IF)
                return Prepare8080Jump2(node->a);
            if (node->operator_code == COP_LAND || node->operator_code == COP_LOR) {
                const bool b = Prepare8080Jump2(node->a);
                const bool a = Prepare8080Jump2(node->b);
                return a || b;
            }
            return false;
        case CNT_MONO_OPERATOR:
            if (node->mono_operator_code == MOP_NOT)
                return Prepare8080Jump2(node->a);
            return false;
        case CNT_IF:
        case CNT_WHILE:
        case CNT_DO:
            return Prepare8080Jump2(node->a);
        case CNT_FOR:
            return Prepare8080Jump2(node->b);
    }
    return false;
}
