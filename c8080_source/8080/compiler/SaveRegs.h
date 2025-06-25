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

#include "../asm/asm2.h"
#include "Compiler.h"

class SaveRegs {
public:
    Asm2 &out;
    bool enabled{};
    AsmRegister reg{};

    SaveRegs(Asm2 &out_, CBuildCase &c, AsmRegister reg_, uint32_t mask) : out(out_), reg(reg_) {
        enabled = (c.regs & mask);
        if (enabled)
            out.push_reg(reg);
    }

    void End() {
        if (enabled) {
            out.pop_reg(reg);
            enabled = false;
        }
    }

    ~SaveRegs() {
        assert(!enabled);
    }
};

class SaveHlLoadDe {
public:
    Asm2 &out;
    bool enabled{};

    SaveHlLoadDe(Asm2 &out_, AsmRegister reg, CBuildCase &c) : out(out_) {
        switch(reg) {
            case R16_HL:
            case R16_DE:
                enabled = (c.regs & U_DE);
                if (enabled)
                    out.push_reg(R16_HL);
                else
                    out.ex_hl_de();
                break;
            case R8_A:
            case R8_D:
                enabled = (c.regs & U_D);
                if (enabled)
                    out.push_reg(R16_AF);
                else
                    out.ld_d_a();
                break;
            default:
                throw std::runtime_error(__PRETTY_FUNCTION__);
        }
    }

    void End() {
        if (enabled) {
            out.pop_reg(R16_DE);
            enabled = false;
        }
    }

    ~SaveHlLoadDe() {
        assert(!enabled);
    }
};
