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

enum AssemblerCommand {
    AC_REMOVED,
    AC_MVI,
    AC_LXI,
    AC_LD_REG_STACK_ADDR,
    AC_LD_REG_ARG_STACK_ADDR,
    AC_STACK_CORRECTION,
    AC_STACK_CORRECTION_RESET,
    AC_LHLD,
    AC_SHLD,
    AC_STA,
    AC_LDA,
    AC_LD_A_PREG,
    AC_LD_PREG_A,
    AC_LD_REG_REG,
    AC_XCHG,
    AC_XTHL,
    AC_RLA,
    AC_RRA,
    AC_RRCA,
    AC_RLCA,
    AC_JMP,
    AC_JMP_CONDITION,
    AC_LABEL,
    AC_PUSH,
    AC_POP,
    AC_RET,
    AC_CALL,
    AC_ALU_A_CONST,
    AC_ALU_A_REG,
    AC_ADD_HL_REG,
    AC_SPHL,
    AC_CPL,
    AC_INC_REG,
    AC_DEC_REG,
    AC_LINE,
    AC_ASSEMBLER,
    AC_REMARK,
    AC_CALL_CONDITION,
    AC_RET_CONDITION,
    AC_IN,
    AC_OUT,
    AC_PCHL,
    AC_STC,
    AC_EI,
    AC_DI,
    AC_NOP,
};

int GetAssemblerCommandSize(enum AssemblerCommand c);
