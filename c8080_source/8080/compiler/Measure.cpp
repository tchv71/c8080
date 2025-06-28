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

void Compiler8080::Measure(CNodePtr &node, AsmRegister reg, CBuildProc proc) {
    MeasureBegin();
    measure_proc = proc;
    const auto prev = out.measure_metric;
    (this->*proc)(node, reg);
    if (prev == out.measure_metric) {
        out.measure = false;  // Canceled
        measure_proc = nullptr;
        return;
    }
    MeasureEnd(node, reg);
}

void Compiler8080::MeasureBegin(AsmRegister reg) {
    if (reg == REG_PREPARE)
        MeasureBegin();
}

void Compiler8080::MeasureBegin() {
    assert(!out.measure);
    out.measure = true;
    out.measure_metric = 0;
    out.measure_regs = 0;
    out.measure_args_metric = 0;
    out.measure_args_id = 0;
}

void Compiler8080::MeasureMid(CNodePtr &node, AsmRegister reg) {
    if (out.measure) {
        CBuildCase &c = node->bi.Get(reg);
        if (!c.able || c.metric > out.measure_metric)
            c.Set(out.measure_regs, out.measure_metric, out.measure_args_id, measure_proc);
    }
}

void Compiler8080::MeasureEnd(CNodePtr &node, AsmRegister reg) {
    MeasureMid(node, reg);
    out.measure = false;
    measure_proc = nullptr;
}

bool Compiler8080::MeasureReset(CNodePtr &node, AsmRegister reg) {
    if (reg == REG_PREPARE) {
        node->bi.no_result.Reset();
        node->bi.main.Reset();
        node->bi.alt.Reset();
        return false;
    }

    CBuildCase &c = node->bi.Get(reg);
    if (c.build == nullptr)
        C_ERROR_INTERNAL(node, "No compilation cases");

    (this->*(c.build))(node, reg);
    return true;
}
