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

#include <string>
#include "ctype.h"
#include <memory>

class CStructItem {
public:
    CType type;
    std::string name;
    uint64_t struct_item_offset{};

    bool operator==(const CStructItem &b) const {
        return type == b.type && name == b.name;
    }

    bool operator!=(const CStructItem &b) const {
        return !(*this == b);
    }

    std::string ToString() const {
        return type.ToString() + " " + name;
    }
};

typedef std::shared_ptr<CStructItem> CStructItemPtr;
