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

#include <stdint.h>

enum AsmRegister {
    REG_NONE,
    R8_A,
    R8_B,
    R8_C,
    R8_D,
    R8_E,
    R8_H,
    R8_L,
    R8_M,
    R16_AF,
    R16_BC,
    R16_DE,
    R16_HL,
    R16_SP,
    R32_DEHL,
    REG_PREPARE
};

static const uint32_t U_ALL = UINT32_MAX;
static const uint32_t U_A = 1u << R8_A;
static const uint32_t U_B = 1u << R8_B;
static const uint32_t U_C = 1u << R8_C;
static const uint32_t U_D = 1u << R8_D;
static const uint32_t U_E = 1u << R8_E;
static const uint32_t U_H = 1u << R8_H;
static const uint32_t U_L = 1u << R8_L;
static const uint32_t U_BC = U_B | U_C;
static const uint32_t U_DE = U_D | U_E;
static const uint32_t U_HL = U_H | U_L;
static const uint32_t U_HLDE = U_DE | U_HL;
static const uint32_t U_DEHL = U_DE | U_HL;

uint32_t RegToUsed(AsmRegister reg);

bool IsAsmRegister16SpAf(AsmRegister reg);
bool IsAsmRegister16Sp(AsmRegister r);
bool IsAsmRegister8(AsmRegister r);

const char *ToString(AsmRegister value);
