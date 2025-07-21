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

#include "../../c/cprogramm.h"

namespace I8080 {

enum CmmName {
    CMM_NAME_FLAG = 1,
    CMM_NAME_REG,
    CMM_NAME_SET_FLAG_C,
    CMM_NAME_INVERT,
    CMM_NAME_COMPARE,
    CMM_NAME_CYCLIC_ROTATE_LEFT,
    CMM_NAME_CYCLIC_ROTATE_RIGHT,
    CMM_NAME_CARRY_ROTATE_LEFT,
    CMM_NAME_CARRY_ROTATE_RIGHT,
    CMM_NAME_PUSH,
    CMM_NAME_POP,
    CMM_NAME_IN,
    CMM_NAME_OUT,
    CMM_NAME_SWAP,
    CMM_NAME_CARRY_ADD,
    CMM_NAME_CARRY_SUB,
    CMM_NAME_ENABLE_INTERRUPTS,
    CMM_NAME_DISABLE_INTERRUPTS,
    CMM_NAME_NOP,
    CMM_NAME_DAA,
};

void RegisterInternalCmmNames(CProgramm &p);

}  // namespace I8080
