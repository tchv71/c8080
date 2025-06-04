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
#include "../tools/cstring.h"

class CMacroizer;  // Declared here to reduce the code amount

class CErrorPosition {
public:
    const char *file_name;
    size_t line;
    size_t column;
    const char *cursor;

    CErrorPosition() {
        file_name = "";
        line = 0;
        column = 0;
        cursor = nullptr;
    }

    CErrorPosition(const CMacroizer &p);  // Declared here to reduce the code amount

    std::string ToString() const;
};

typedef const CErrorPosition &CConstErrorPosition;
