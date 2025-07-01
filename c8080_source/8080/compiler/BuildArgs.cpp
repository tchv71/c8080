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
#include "PushMainPopDe.h"

enum BuildArgsMode {
    BAM_ANY,
    BAM_A_MAIN,
    BAM_B_MAIN,
};

bool Compiler8080::Case_Args2_MM(CNodePtr &a, CNodePtr &b, AsmRegister main, AsmRegister alt, unsigned bam) {
    if (bam == BAM_A_MAIN && main == R8_A) {
        Build(a, R8_A);
        out.push_reg(R16_AF);
        Build(b, R8_A);
        out.ld_r8_r8(R8_D, R8_A);
        out.pop_reg(R16_AF);
        return true;
    }

    Build(a, main);
    PushMainPopDe sr(out, main, b->compiler.Get(main));  // or LD D, A or EX HL, DE
    Build(b, main);
    sr.PopDe();
    if (bam == BAM_A_MAIN)
        out.ex_hl_de();
    return true;
}

bool Compiler8080::Case_Args2_MA(CNodePtr &a, CNodePtr &b, AsmRegister main, AsmRegister alt, unsigned bam) {
    if (!b->compiler.alt.able)
        return false;

    if (bam == BAM_B_MAIN && main == R8_A) {
        Build(a, R8_A);
        out.push_reg(R16_AF);
        Build(b, R8_D);
        out.ld_r8_r8(R8_A, R8_D);
        out.pop_reg(R16_DE);
        return true;
    }

    Build(a, main);
    SaveReg sr(out, b->compiler.alt, main);
    Build(b, alt);
    sr.Pop();
    if (bam == BAM_B_MAIN)
        out.ex_hl_de();
    return true;
}

bool Compiler8080::Case_Args2_AM(CNodePtr &a, CNodePtr &b, AsmRegister main, AsmRegister alt, unsigned bam) {
    if (!a->compiler.alt.able)
        return false;

    if (bam == BAM_A_MAIN && main == R8_A) {
        Build(a, R8_D);
        out.push_reg(R16_DE);
        Build(b, R8_A);
        out.ld_r8_r8(R8_D, R8_A);
        out.pop_reg(R16_AF);
        return true;
    }

    Build(a, alt);
    SaveReg sr(out, b->compiler.main, alt);
    Build(b, main);
    sr.Pop();
    if (bam == BAM_A_MAIN)
        out.ex_hl_de();
    return true;
}

void Compiler8080::BuildArgs2(const CNodePtr &node, AsmRegister reg, CNodePtr &a, CNodePtr &b, AsmRegister main,
                              AsmRegister alt, bool can_swap) {
    if (out.measure) {
        assert(out.measure_metric == 0);
        assert(out.measure_regs == 0);
        out.measure_args_id = 0;

        MeasureArgsBegin();
        if (Case_Args2_MM(a, b, main, alt, can_swap ? BAM_ANY : BAM_A_MAIN))
            MeasureArgsEnd(0x01);
        MeasureArgsBegin();
        if (Case_Args2_AM(a, b, main, alt, can_swap ? BAM_ANY : BAM_A_MAIN))
            MeasureArgsEnd(0x02);
        MeasureArgsBegin();
        if (Case_Args2_MA(a, b, main, alt, can_swap ? BAM_ANY : BAM_A_MAIN))
            MeasureArgsEnd(0x03);
        MeasureArgsBegin();
        if (Case_Args2_MM(b, a, main, alt, can_swap ? BAM_ANY : BAM_B_MAIN))
            MeasureArgsEnd(0x11);
        MeasureArgsBegin();
        if (Case_Args2_AM(b, a, main, alt, can_swap ? BAM_ANY : BAM_B_MAIN))
            MeasureArgsEnd(0x12);
        MeasureArgsBegin();
        if (Case_Args2_MA(b, a, main, alt, can_swap ? BAM_ANY : BAM_B_MAIN))
            MeasureArgsEnd(0x13);

        assert(out.measure_args_id != 0);
        out.measure_metric = out.measure_args_metric;
        out.measure_regs = out.measure_args_regs;
        return;
    }

    unsigned args = node->compiler.Get(reg).args;
    switch (args) {
        case 0x01:
            Case_Args2_MM(a, b, main, alt, can_swap ? BAM_ANY : BAM_A_MAIN);
            break;
        case 0x02:
            Case_Args2_AM(a, b, main, alt, can_swap ? BAM_ANY : BAM_A_MAIN);
            break;
        case 0x03:
            Case_Args2_MA(a, b, main, alt, can_swap ? BAM_ANY : BAM_A_MAIN);
            break;
        case 0x11:
            Case_Args2_MM(b, a, main, alt, can_swap ? BAM_ANY : BAM_B_MAIN);
            break;
        case 0x12:
            Case_Args2_AM(b, a, main, alt, can_swap ? BAM_ANY : BAM_B_MAIN);
            break;
        case 0x13:
            Case_Args2_MA(b, a, main, alt, can_swap ? BAM_ANY : BAM_B_MAIN);
            break;
        default:
            C_ERROR_INTERNAL(node->e, __PRETTY_FUNCTION__);
    }
}
