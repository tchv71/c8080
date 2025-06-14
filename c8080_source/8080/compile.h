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

#include "../c/parser/cparser.h"

enum OutputFormat {
    OF_I1080,
    OF_CPM,
};

bool ParseOutputFormat(enum OutputFormat &result, const char *str);
void RegisterInternalNames8080(CProgramm &programm);
void Compile_8080(CParser &c, CProgramm &programm, OutputFormat output_format, const std::string &output_file_bin,
                  CString asm_file_name);
