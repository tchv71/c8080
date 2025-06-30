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
#include "../../prepare/prepare.h"
#include "jump.h"

static const PrepareFunctionType prepare_function_list[] = {
    Prepare8080Const,
    Prepare8080SetOperators,
    Prepare8080Fast8BitMath,
    Prepare8080Fast8BitOptimization,
    PrepareCompareOperators8080,
    Prepare8080RemoveDead,
    Prepare8080IncDec,
    Prepare8080Sub16ToAdd16,
    nullptr,
};

void Prepare8080Function(CProgramm *programm, CNodePtr &node, Asm2 *out) {
    PrepareFunction(*programm, node->variable, prepare_function_list, out);
    Prepare8080Jump(&node);
}

void Prepare8080Variable(CProgramm &programm, CNodePtr &node, Asm2 *out) {
    CVariablePtr no_function;
    Prepare p(programm, no_function, prepare_function_list);
    p.out = out;
    PrepareInt(p, &node);
}
