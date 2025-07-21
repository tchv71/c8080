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

#pragma once

#include "../../c/cvariable.h"
#include "../../c/tools/cthrow.h"
#include "../asm/asmregister.h"
#include "../asm/asmalu.h"
#include "../asm/asmlabel.h"

namespace I8080 {

class Compiler;

typedef bool (Compiler::*CBuildProc)(CNodePtr &node, AsmRegister reg);

class CBuildCase {
public:
    bool able{};
    uint32_t regs{};
    int metric{};
    CBuildProc build{};
    unsigned args{};

    void Reset() {
        able = false;
        regs = 0;
        metric = 0;
        build = nullptr;
        args = 0;
    }

    void Set(uint32_t r, int m, unsigned a, CBuildProc b) {
        able = true;
        regs = r;
        metric = m;
        build = b;
        args = a;
    }
};

struct CNode8080 {
    std::vector<CVariablePtr> used_variables;  // For CNT_CONST
    AsmLabel *label{};                         // For CNT_CASE CNT_DEFAULT CNT_LABEL CNT_GOTO
    AsmAlu alu{};                              // CNT_OPERATOR
    bool rearrange{};                          // CNT_OPERATOR

    CBuildCase main;  // A, HL, DEHL
    CBuildCase alt;   // D, DE
    CBuildCase no_result;

    CBuildCase &Get(AsmRegister reg) {
        switch (reg) {
            case R8_D:
            case R16_DE:
                return alt;
            case REG_NONE:
                return no_result.able ? no_result : main;
            default:
                return main;
        }
    }
};

}  // namespace I8080
