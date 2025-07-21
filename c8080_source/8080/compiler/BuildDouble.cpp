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

#include "Compiler.h"
#include "SaveReg.h"

namespace I8080 {

bool Compiler::BuildDouble(CNodePtr &node, AsmRegister reg, CNodePtr &a, AsmRegister a_reg, CNodePtr &b,
                           AsmRegister b_reg) {
    CBuildCase a_case = a->compiler.Get(a_reg);
    CBuildCase b_case = b->compiler.Get(b_reg);

    if (out.measure) {
        if (!a_case.able || !b_case.able)
            return false;

        assert(out.measure_metric == 0);
        assert(out.measure_regs == 0);
        out.measure_args_id = 0;

        MeasureArgsBegin();
        Case_Double(a, a_reg, b, b_reg, b_case);
        MeasureArgsEnd(1);

        MeasureArgsBegin();
        Case_Double(b, b_reg, a, a_reg, a_case);
        MeasureArgsEnd(2);

        out.measure_metric = out.measure_args_metric;
        out.measure_regs = out.measure_args_regs;
        return true;
    }

    assert(a_case.able);
    assert(b_case.able);

    switch (node->compiler.Get(reg).args) {
        case 1:
            Case_Double(a, a_reg, b, b_reg, b_case);
            break;
        case 2:
            Case_Double(b, b_reg, a, a_reg, a_case);
            break;
        default:
            C_ERROR_INTERNAL(node, __PRETTY_FUNCTION__);
    }

    return true;
}

void Compiler::Case_Double(CNodePtr &a, AsmRegister a_reg, CNodePtr &b, AsmRegister b_reg, CBuildCase &b_case) {
    Build(a, a_reg);
    SaveReg sr(out, b_case, a_reg);
    Build(b, b_reg);
    sr.Pop();
}

}  // namespace I8080
