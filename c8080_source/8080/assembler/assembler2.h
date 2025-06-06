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

#include "8080_assembler.h"

class Assembler2 : public Assembler {
public:
    void ld_pointer_hl(CString string) {
        add(AC_LD_PCONST_HL, string);
    }

    void ld_pointer_hl(uint16_t number) {
        add(AC_LD_PCONST_HL, number);
    }

    void ld_a_pointer(CString string) {
        add(AC_LD_A_PCONST, string);
    }

    void ex_hl_de() {
        ChangedReg(R32_DEHL);
        add(AC_XCHG);
    }

    void ex_psp_hl() {
        ChangedReg(R16_HL);
        add(AC_XTHL);
    }

    void push_reg(Reg reg) {
        add(AC_PUSH, reg);
    }

    void pop_reg(Reg reg) {
        ChangedReg(reg);
        add(AC_POP, reg);
    }

    void rla() {
        ChangedReg(R8_A);
        add(AC_RLA);
    }

    void rra() {
        ChangedReg(R8_A);
        add(AC_RRA);
    }

    void rrca() {
        ChangedReg(R8_A);
        add(AC_RRCA);
    }

    void rlca() {
        ChangedReg(R8_A);
        add(AC_RLCA);
    }

    void ei() {
        add(AC_EI);
    }

    void di() {
        add(AC_DI);
    }

    void pchl() {
        add(AC_PCHL);
    }

    void stc() {
        add(AC_STC);
    }

    void ld_sp_hl() {
       ChangedReg(R16_SP);
       add(AC_LD_SP_HL);
    }

    void cpl() {
        ChangedReg(R16_AF);
        add(AC_CPL);
    }

    void call2(CString string) {
        AllRegistersChanged();
        add(AC_CALL, string);
    }

    void call2(uint16_t number) {
        AllRegistersChanged();
        add(AC_CALL, number);
    }

    void ld_pointer_a(CString string) {
        add(AC_LD_PCONST_A, string);
    }

    void ld_pointer_a(uint16_t number) {
        add(AC_LD_PCONST_A, number);
    }

    void rotate(AssemblerCommand opcode) {
        assert(opcode == AC_RLA || opcode == AC_RRA || opcode == AC_RLCA || opcode == AC_RRCA);
        ChangedReg(R8_A);
        add(opcode);
    }

    void dec_reg(Reg reg) {
        ChangedReg(reg);
        add(AC_DEC_REG, reg);
    }

    void inc_reg(Reg reg) {
        ChangedReg(reg);
        add(AC_INC_REG, reg);
    }

    void in(CString string) {
        ChangedReg(R8_A);
        add(AC_IN, string);
    }

    void out(CString string) {
        add(AC_OUT, string);
    }

    void Remark(CString string) {
        add(AC_REMARK, string);
    }

    void label(CString string) {
        add(AC_LABEL, string);
    }

    void label(AssemblerLabel *label) {
        if (measure_ == nullptr) {
            add(AC_LABEL, label);
            label->used++;
            label->destination = lines_.size();
        }
    }

    void jmp_label(AssemblerLabel *label) {
        if (measure_ == nullptr) {
            add(AC_JMP, label);
            label->used++;
        }
    }

    void jmp(CString string) {
        add(AC_JMP, string);
    }

    void ld_hl_pointer(CString string) {
        ChangedReg(R16_HL);
        add(AC_LD_HL_PCONST, string);
    }

    void add_hl_reg(Reg reg) {
        assert(reg == R16_BC || reg == R16_DE || reg == R16_HL || reg == R16_SP);
        add(AC_ADD_HL_REG, reg);
    }

    // Simple commands

    void ld_bc_number(uint16_t number) {
        ld_reg_number(R16_BC, number);
    }

    void ld_de_number(uint16_t number) {
        ld_reg_number(R16_DE, number);
    }

    void ld_hl_number(uint16_t number) {
        ld_reg_number(R16_HL, number);
    }

    void ld_a_pbc() {
        ld_a_preg(R16_BC);
    }

    void ld_a_pde() {
        ld_a_preg(R16_DE);
    }

    void ld_a_phl() {
        ld_a_preg(R16_HL);
    }

    void ld_l_phl() {
        ld_reg_reg(R8_L, R8_M);
    }

    void ld_d_phl() {
        ld_reg_reg(R8_D, R8_M);
    }

    void ld_e_phl() {
        ld_reg_reg(R8_E, R8_M);
    }

    void ld_phl_a() {
        ld_reg_reg(R8_M, R8_A);
    }

    void ld_l_a() {
        ld_reg_reg(R8_L, R8_A);
    }

    void ld_d_a() {
        ld_reg_reg(R8_D, R8_A);
    }

    void ld_a_l() {
        ld_reg_reg(R8_A, R8_L);
    }

    void ld_a_h() {
        ld_reg_reg(R8_A, R8_H);
    }

    void ld_h_d() {
        ld_reg_reg(R8_H, R8_D);
    }

    void ld_h_l() {
        ld_reg_reg(R8_H, R8_L);
    }

    void ld_a_number(uint8_t value) {
        ld_reg_number(R8_A, value);
    }

    void ld_d_number(uint8_t value) {
        ld_reg_number(R8_D, value);
    }

    void jz_label(AssemblerLabel *label) {
        jmp_condition_label(JC_Z, label);
    }

    void jnz_label(AssemblerLabel *label) {
        jmp_condition_label(JC_NZ, label);
    }

    void jc_label(AssemblerLabel *label) {
        jmp_condition_label(JC_C, label);
    }

    void jnc_label(AssemblerLabel *label) {
        jmp_condition_label(JC_NC, label);
    }

    void jm_label(AssemblerLabel *label) {
        jmp_condition_label(JC_M, label);
    }

    void jp_label(AssemblerLabel *label) {
        jmp_condition_label(JC_P, label);
    }

    void push_af() {
        push_reg(R16_AF);
    }

    void pop_af() {
        pop_reg(R16_AF);
    }

    void push_hl() {
        push_reg(R16_HL);
    }

    void push_de() {
        push_reg(R16_DE);
    }

    void pop_hl() {
        pop_reg(R16_HL);
    }

    void pop_bc() {
        pop_reg(R16_BC);
    }

    void pop_de() {
        pop_reg(R16_DE);
    }

    void ld_phl_e() {
        ld_reg_reg(R8_M, R8_E);
    }

    void ld_phl_d() {
        ld_reg_reg(R8_M, R8_D);
    }

    void ld_phl_reg(Reg reg) {
        ld_reg_reg(R8_M, reg);
    }

    void ld_phl_const(CString value) {
        ld_reg_string(R8_M, value);
    }

    void ld_phl_number(uint8_t value) {
        ld_reg_number(R8_M, value);
    }

    void inc_de() {
        inc_reg(R16_DE);
    }

    void inc_hl() {
        inc_reg(R16_HL);
    }

    void dec_de() {
        dec_reg(R16_DE);
    }

    void dec_hl() {
        dec_reg(R16_HL);
    }

    void inc_phl() {
        inc_reg(R8_M);
    }

    void dec_phl() {
        dec_reg(R8_M);
    }

    void cmp_a_d() {
        alu_a_reg(ALU_CMP, R8_D);
    }

    void cmp_a_string(CString string) {
        alu_a_string(ALU_CMP, string);
    }

    void add_a_const(CString string) {
        alu_a_string(ALU_ADD, string);
    }

    void sub_a_const(CString string) {
        alu_a_string(ALU_SUB, string);
    }

    void and_a_const(CString string) {
        alu_a_string(ALU_AND, string);
    }

    void and_a_number(uint8_t number) {
        alu_a_number(ALU_AND, number);
    }

    void or_a_const(CString string) {
        alu_a_string(ALU_OR, string);
    }

    void xor_a_const(CString string) {
        alu_a_string(ALU_XOR, string);
    }

    void ld_de_hl() {
        ld_reg_reg(R8_D, R8_H);
        ld_reg_reg(R8_E, R8_L);
    }

    void inc_a() {
        inc_reg(R8_A);
    }

    void dec_a() {
        dec_reg(R8_A);
    }

    void push_de_hl() {
        push_reg(R16_DE);
        push_reg(R16_HL);
    }

    void pop_hl_de() {
        pop_reg(R16_HL);
        pop_reg(R16_DE);
    }

    void ld_hl_bc() {
        ld_reg_reg(R8_H, R8_B);
        ld_reg_reg(R8_L, R8_C);
    }

    void ld_bc_hl() {
        ld_reg_reg(R8_B, R8_H);
        ld_reg_reg(R8_C, R8_L);
    }

    void add_a_d() {
        alu_a_reg(ALU_ADD, R8_D);
    }

    void add_a_a() {
        alu_a_reg(ALU_ADD, R8_A);
    }

    void sub_a_d() {
        alu_a_reg(ALU_SUB, R8_D);
    }

    void and_a_d() {
        alu_a_reg(ALU_AND, R8_D);
    }

    void or_a_d() {
        alu_a_reg(ALU_OR, R8_D);
    }

    void or_a_e() {
        alu_a_reg(ALU_OR, R8_E);
    }

    void xor_a_d() {
        alu_a_reg(ALU_XOR, R8_D);
    }

    void add_hl_de() {
        add_hl_reg(R16_DE);
    }

    void add_hl_hl() {
        add_hl_reg(R16_HL);
    }

    void add_hl_sp() {
        add_hl_reg(R16_SP);
    }

    void or_a_a() {
        alu_a_reg(ALU_OR, R8_A);
    }

    void or_a_h() {
        alu_a_reg(ALU_OR, R8_H);
    }

    void ld_h_number(uint8_t number) {
        ld_reg_number(R8_H, number);
    }

    void ld_l_number(uint8_t number) {
        ld_reg_number(R8_L, number);
    }

    void ld_d_offset(CString string) {
        ld_reg_string(R8_D, string);
    }

    void ld_bc_offset(CString string) {
        ld_reg_string(R16_BC, string);
    }

    void ld_de_offset(CString string) {
        ld_reg_string(R16_DE, string);
    }

    void ld_hl_offset(CString string) {
        ld_reg_string(R16_HL, string);
    }
};
