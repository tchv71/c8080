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
#include "index.h"
#include "../c/tools/ccalcconst.h"
#include "../c/tools/cthrow.h"
#include "staticstack.h"

bool DeleteNodeSaveType(CNodePtr &node, char c) {
    CType type = node->ctype;
    DeleteNode(node, c);
    if (node->ctype.GetAsmType() != type.GetAsmType())
        CThrow(node, "Internal error in " + std::string(__PRETTY_FUNCTION__));
    node->ctype = type;
    return true;
}

typedef bool (*PrepareFunctionType)(Prepare &p, CNodePtr &node);

static const PrepareFunctionType prepare_function_list[] = {
    PrepareRemoveUselessOperations,
    PrepareReplaceDivMulWithShift,
    PrepareStructItem,
    PrepareArrayElement,
    PrepareLocalVariablesInit,
    PrepareAddrDeaddr,
    PrepareStaticLoadVariable,
    PrepareLoadVariable,
    PrepareStaticArgumentsCall,
    PrepareAddWithStackAddress,
};

bool PrepareInt(Prepare &p, CNodePtr *pnode) {
    bool result_changed = false;
    while (*pnode != nullptr) {
        bool changed;
        do {
            changed = PrepareInt(p, &(*pnode)->a);
            changed |= PrepareInt(p, &(*pnode)->b);
            changed |= PrepareInt(p, &(*pnode)->c);
            changed |= PrepareInt(p, &(*pnode)->d);
            changed |= CCalcConst(*pnode, false);

            for (auto &i : prepare_function_list) {
                changed |= i(p, *pnode);
                if (*pnode == nullptr)
                    break;
            }

            if (*pnode == nullptr)
                break;

            for (auto i = p.list; *i; i++) {
                changed |= (*i)(p, *pnode);
                if (*pnode == nullptr)
                    break;
            }

            result_changed |= changed;
        } while (changed);

        pnode = &(*pnode)->next_node;
    }
    return result_changed;
}

void PrepareFunction(CProgramm &programm, CVariablePtr &f, const PrepareFunctionType *list) {
    Prepare p(programm, f, list);
    if (p.function) {
        PrepareFunctionStaticStack(p);
        PrepareInt(p, &p.function->body->a);
    } else {
        PrepareInt(p, &p.function->body);
    }
}
