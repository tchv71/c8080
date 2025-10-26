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

class CPointer {
public:
    uint64_t array_size{};
    bool flag_const{};
    bool flag_volatile{};
    bool flag_restrict{};
    bool is_array{};

    CPointer() {
    }

    CPointer(uint64_t _array_size) {
        array_size = _array_size;
        is_array = true;
    }

    bool operator==(const CPointer &b) const {
        return array_size == b.array_size && flag_const == b.flag_const && flag_volatile == b.flag_volatile &&
               flag_restrict == b.flag_restrict && is_array == b.is_array;
    }

    bool operator!=(const CPointer &b) const {
        return !(*this == b);
    }

    void SetArray(uint64_t size) {
        is_array = true;
        array_size = size;
    }

    void ResetArray() {
        is_array = false;
        array_size = 0;
    }
};
