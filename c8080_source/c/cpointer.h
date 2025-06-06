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
    uint64_t count{};
    bool flag_const{};
    bool flag_volatile{};
    bool flag_restrict{};

    bool operator==(const CPointer &b) const {
        return count == b.count && flag_const == b.flag_const && flag_volatile == b.flag_volatile &&
               flag_restrict == b.flag_restrict;
    }

    bool operator!=(const CPointer &b) const {
        return !(*this == b);
    }
};
