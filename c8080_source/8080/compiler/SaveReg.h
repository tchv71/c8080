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

#include "../asm/asm.h"
#include "Compiler.h"

namespace I8080 {

class SaveReg {
public:
    Asm &out;
    bool enabled{};
    AsmRegister reg{};

    SaveReg(Asm &out_, const CBuildCase &c, AsmRegister reg_) : out(out_), reg(reg_) {
        enabled = (c.regs & RegToUsed(reg));
        if (enabled)
            out.push_reg(reg);
    }

    void Pop() {
        if (enabled) {
            out.pop_reg(reg);
            enabled = false;
        }
    }

    ~SaveReg() {
        assert(!enabled);
    }
};

}  // namespace I8080
