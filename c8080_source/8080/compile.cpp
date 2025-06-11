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

#include "compile.h"
#include "8080_compile.h"
#include "8080_prepare.h"

void RegisterInternalNames8080(CProgramm &programm) {
    programm.output_names["__begin"] = 1;
    programm.output_names["__entry"] = 1;
    programm.output_names["__bss"] = 1;
    programm.output_names["__end"] = 1;
    // TODO: Зарезервированные имена ассемблера
}

void Compile8080_(CParser &c, CProgramm &programm, OutputFormat output_format, const std::string &output_file_bin,
                  CString asm_file_name) {
    Prepare8080(&programm, programm.first_node, 0);

    Compile8080 compile8080;
    compile8080.Compile(c, programm, output_format, output_file_bin, asm_file_name);
}

bool ParseOutputFormat(enum OutputFormat &result, const char *str) {
    if (0 == strcasecmp(str, "i1080")) {
        result = OF_I1080;
        return true;
    }
    if (0 == strcasecmp(str, "cpm")) {
        result = OF_CPM;
        return true;
    }
    return false;
}
