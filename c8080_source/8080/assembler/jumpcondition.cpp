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

#include "jumpcondition.h"
#include <array>
#include <stdexcept>

bool InvertJumpCondition(JumpCondition &jump_condition) {
    switch (jump_condition) {
        case JC_Z:
            jump_condition = JC_NZ;
            return true;
        case JC_NZ:
            jump_condition = JC_Z;
            return true;
        case JC_C:
            jump_condition = JC_NC;
            return true;
        case JC_NC:
            jump_condition = JC_C;
            return true;
        case JC_P:
            jump_condition = JC_M;
            return true;
        case JC_M:
            jump_condition = JC_P;
            return true;
        default:
            return false;
    }
}

const char *ToString(JumpCondition value) {
    static const char *strings[] = {"z", "nz", "c", "nc", "p", "m", "pe", "po"};

    if (value >= std::size(strings))
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " " + std::to_string(int(value)));

    return strings[value];
}
