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

#include "prepare.h"
#include "names.h"
#include "../prepare/index.h"
#include "../../prepare/prepare.h"

namespace I8080 {

bool PrepareCmmVariables(Prepare &, CNodePtr &node) {
    if (node->type != CNT_LOAD_VARIABLE)
        return false;

    CVariablePtr &v = node->variable;

    if (v->c.internal_cmm_name != 0)
        return false;

    if (v->type.pointers.size() > 0 && v->type.pointers.back().count > 0) {
        // Replace the global array with a constant.
        // For example, if "static int a[4]; int *b = a;", then the value of b is known at compile time.
        node->type = CNT_CONST;
        assert(!v->output_name.empty());
        node->text = v->output_name;
        node->compiler.used_variables.push_back(v);
    } else {
        // Replace the address of the global variable with a constant with dereference
        node->type = CNT_MONO_OPERATOR;
        node->mono_operator_code = MOP_DEADDR;
        node->a = CNODE({CNT_CONST});
        node->a->e = node->e;
        node->a->text = v->output_name;
        node->a->ctype = v->type;
        node->a->ctype.pointers.push_back(CPointer{0});
        node->a->compiler.used_variables.push_back(v);
    }
    return true;
}

static bool PrepareCmmConvert(Prepare &, CNodePtr &node) {
    if (node->type == CNT_CONVERT) {
        DeleteNode(node, 'a');  // TODO: Check
        return true;
    }
    return false;
}

static const PrepareFunctionType prepare_function_list_cmm[] = {
    PrepareConst,
    PrepareCmmVariables,
    PrepareCmmConvert,
    nullptr,
};

void PrepareCmm(Asm &out, CProgramm &programm, CNodePtr &node) {
    Prepare p(programm, out, node->variable, prepare_function_list_cmm);
    PrepareInt(p, &p.function->body->a);
}

}  // namespace I8080
