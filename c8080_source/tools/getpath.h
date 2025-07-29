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

#include "cstring.h"

static inline std::string GetPath(CString file_name) {
    size_t pos = file_name.rfind('/');
#ifdef __MINGW32__
    const size_t pos1 = file_name.rfind('\\');
    if (pos1 != file_name.npos && (pos == file_name.npos || pos < pos1))
        pos = pos1;
#endif
    return pos == file_name.npos ? "" : file_name.substr(0, pos);
}
