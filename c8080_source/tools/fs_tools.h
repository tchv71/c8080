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

#include <stddef.h>
#include <stdint.h>
#include <functional>
#include <vector>
#include "cstring.h"

namespace FsTools {

void LoadFile(CString file_name, std::function<void *(size_t)> allocate);
void LoadFile(CString file_name, size_t max_file_size, std::vector<uint8_t> &out);
void LoadFile(CString file_name, size_t max_file_size, std::string &out);
void SaveFile(CString file_name, const void *data, size_t size);
void SaveFile(CString file_name, const std::vector<uint8_t> &in);
void SaveFile(CString file_name, CString in);

}  // namespace FsTools
