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
#include "prepareint.h"
#include "../c/tools/ccalcconst.h"
#include "../c/tools/cthrow.h"

bool DeleteNodeSaveType(CNodePtr &node, char c) {
    CType type = node->ctype;
    DeleteNode(node, c);
    if (node->ctype.GetAsmType() != type.GetAsmType())
        CThrow(node, "Internal error in " + std::string(__PRETTY_FUNCTION__));
    node->ctype = type;
    return true;
}

typedef bool (*PrepareFunctionType)(CNodePtr &node);

static const PrepareFunctionType prepare_function_list[] = {
    PrepareUselessOperations, PrepareReplaceDivMul,      PrepareStructItem,
    PrepareArrayElement,      PrepareLocalVariablesInit, PrepareAddrDeaddr,
};

static bool PrepareInt(CNodePtr &node) {
    if (node == nullptr)
        return false;

    bool result_changed = false;
    for (;;) {
        bool changed = CCalcConst(node);

        changed |= PrepareInt(node->a);
        changed |= PrepareInt(node->b);
        changed |= PrepareInt(node->c);
        changed |= PrepareInt(node->d);

        for (auto &i : prepare_function_list) {
            changed |= i(node);
            if (node == nullptr)
                break;
        }

        result_changed |= changed;

        if (node == nullptr || !changed)
            break;
    }

    if (node)
        result_changed |= PrepareInt(node->next_node);  // TODO: Recursion

    return result_changed;
}

void Prepare(CProgramm &c) {
    for (CNodePtr i = c.first_node; i; i = i->next_node)
        if (i->variable && i->variable->body && i->variable->type.IsFunction())
            PrepareInt(i->variable->body->a);
}
