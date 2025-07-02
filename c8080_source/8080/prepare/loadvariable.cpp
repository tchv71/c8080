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
#include "prepare.h"
#include "../../c/tools/makecnode.h"

bool Prepare8080LoadVariable(Prepare &p, CNodePtr &node) {
    if (node->type == CNT_LOAD_VARIABLE) {
        CVariablePtr &v = node->variable;
        assert(v != nullptr);
        assert(!v->is_stack_variable);
        assert(!v->output_name.empty());

        Prepare8080Variable(p.programm, v, *p.out);

        if (v->type.IsConst() && v->body && v->body->IsConstNode()) {
            node->c = CopyNode(node->variable->body);
            DeleteNode(node, 'c');
            return true;
        }

        node->type = CNT_CONST;
        node->text = v->output_name;
        node->compiler.used_variables.push_back(v);
        bool a = !v->type.IsArray();
        node->variable = nullptr;

        if (a) {
            node = MakeCNodeDeaddr(node);
            node->a->ctype.pointers.push_back(CPointer{0});
        }
        return true;
    }
    return false;
}
